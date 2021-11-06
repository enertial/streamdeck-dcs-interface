//==============================================================================
/**
@file       StreamdeckInterface.cpp

@brief      Based off of Elgato's CPU plugin example

@copyright  (c) 2018, Corsair Memory, Inc.
                        This source code is licensed under the MIT-style license found in the LICENSE file.

**/
//==============================================================================

#include "StreamdeckInterface.h"

#include <atomic>

#include "ElgatoSD/EPLJSONUtils.h"
#include "ElgatoSD/ESDConnectionManager.h"
#include "SimulatorInterface/DcsIdLookup.h"
#include "SimulatorInterface/SimulatorInterfaceParameters.h"

#include "Vendor/json/src/json.hpp"
using json = nlohmann::json;

class CallBackTimer
{
  public:
    CallBackTimer() : _execute(false) {}

    ~CallBackTimer()
    {
        if (_execute.load(std::memory_order_acquire)) {
            stop();
        };
    }

    void stop()
    {
        _execute.store(false, std::memory_order_release);
        if (_thd.joinable())
            _thd.join();
    }

    void start(int interval, std::function<void(void)> func)
    {
        if (_execute.load(std::memory_order_acquire)) {
            stop();
        };
        _execute.store(true, std::memory_order_release);
        _thd = std::thread([this, interval, func]() {
            while (_execute.load(std::memory_order_acquire)) {
                func();
                std::this_thread::sleep_for(std::chrono::milliseconds(interval));
            }
        });
    }

    bool is_running() const noexcept { return (_execute.load(std::memory_order_acquire) && _thd.joinable()); }

  private:
    std::atomic<bool> _execute;
    std::thread _thd;
};

StreamdeckInterface::StreamdeckInterface()
{
    mTimer = new CallBackTimer();
    mTimer->start(10, [this]() { this->UpdateFromGameState(); });
}

StreamdeckInterface::~StreamdeckInterface()
{
    if (mTimer != nullptr) {
        mTimer->stop();

        delete mTimer;
        mTimer = nullptr;
    }
}

SimulatorConnectionSettings StreamdeckInterface::get_connection_settings(const json &global_settings)
{
    const std::string ip_address_request = EPLJSONUtils::GetStringByName(global_settings, "ip_address");
    const std::string listener_port_request = EPLJSONUtils::GetStringByName(global_settings, "listener_port");
    const std::string send_port_request = EPLJSONUtils::GetStringByName(global_settings, "send_port");
    const std::string multicast_address_request = ""; // TODO: add multicast option to javascript UI.
    const bool user_settings_valid =
        (!ip_address_request.empty() && !listener_port_request.empty() && !send_port_request.empty());

    SimulatorConnectionSettings connection_settings;
    if (user_settings_valid) {
        connection_settings.rx_port = listener_port_request;
        connection_settings.tx_port = send_port_request;
        connection_settings.ip_address = ip_address_request;
        connection_settings.multicast_address = multicast_address_request;
    } else {
        connection_settings.rx_port = DEFAULT_LISTENER_PORT;
        connection_settings.tx_port = DEFAULT_SEND_PORT;
        connection_settings.ip_address = DEFAULT_IP_ADDRESS;
        connection_settings.multicast_address = DEFAULT_MULTICAST_ADDRESS;
    }
    return connection_settings;
}

void StreamdeckInterface::DidReceiveGlobalSettings(const json &inPayload)
{
    json settings;
    EPLJSONUtils::GetObjectByName(inPayload, "settings", settings);
    SimulatorConnectionSettings connection_settings = get_connection_settings(settings);

    // If settings have changed, close BaseSimulatorInterface so it can be re-opened with new connection.
    if (simulator_interface_ && !simulator_interface_.value().connection_settings_match(connection_settings)) {
        simulator_interface_ = std::nullopt;
    }

    // Create first instance of Simulator Interface only done under DidReceiveGlobalSettings to allow for any stored
    // settings to be used before binding socket to default port values.
    if (!simulator_interface_.has_value()) {
        try {
            simulator_interface_.emplace(connection_settings);
        } catch (const std::exception &e) {
            mConnectionManager->LogMessage("Caught Exception While Opening Connection: " + std::string(e.what()));
        }
    }
}

void StreamdeckInterface::UpdateFromGameState()
{
    //
    // Warning: UpdateFromGameState() is running in the timer thread
    //

    if (simulator_interface_) {
        // Update the Simulator game state in memory, then update each Streamdeck button context.
        simulator_interface_.value().update_simulator_state();

        if (mConnectionManager != nullptr) {
            mVisibleContextsMutex.lock();
            for (auto &elem : mVisibleContexts) {
                elem.second->updateContextState(*simulator_interface_, mConnectionManager);
            }
            mVisibleContextsMutex.unlock();
        }
    }
}

void StreamdeckInterface::KeyDownForAction(const std::string &inAction,
                                           const std::string &inContext,
                                           const json &inPayload,
                                           const std::string &inDeviceID)
{
    if (simulator_interface_) {
        mVisibleContextsMutex.lock();
        mVisibleContexts[inContext]->handleButtonPressedEvent(*simulator_interface_, mConnectionManager, inPayload);
        mVisibleContextsMutex.unlock();
    }
}

void StreamdeckInterface::KeyUpForAction(const std::string &inAction,
                                         const std::string &inContext,
                                         const json &inPayload,
                                         const std::string &inDeviceID)
{

    if (simulator_interface_) {
        mVisibleContextsMutex.lock();
        mVisibleContexts[inContext]->handleButtonReleasedEvent(*simulator_interface_, mConnectionManager, inPayload);
        mVisibleContextsMutex.unlock();
    }
}

void StreamdeckInterface::WillAppearForAction(const std::string &inAction,
                                              const std::string &inContext,
                                              const json &inPayload,
                                              const std::string &inDeviceID)
{
    // Remember the context.
    mVisibleContextsMutex.lock();
    json settings;
    EPLJSONUtils::GetObjectByName(inPayload, "settings", settings);
    mVisibleContexts[inContext] = streamdeck_context_factory.create(inAction, inContext, settings);
    if (simulator_interface_) {
        mVisibleContexts[inContext]->forceSendState(mConnectionManager);
    }
    mVisibleContextsMutex.unlock();
}

void StreamdeckInterface::WillDisappearForAction(const std::string &inAction,
                                                 const std::string &inContext,
                                                 const json &inPayload,
                                                 const std::string &inDeviceID)
{
    // Remove the context.
    mVisibleContextsMutex.lock();
    mVisibleContexts.erase(inContext);
    mVisibleContextsMutex.unlock();
}

void StreamdeckInterface::DeviceDidConnect(const std::string &inDeviceID, const json &inDeviceInfo)
{
    // Request global settings from Streamdeck.
    if (mConnectionManager != nullptr) {
        mConnectionManager->GetGlobalSettings();
    }
}

void StreamdeckInterface::DeviceDidDisconnect(const std::string &inDeviceID)
{
    // Nothing to do.
}

void StreamdeckInterface::SendToPlugin(const std::string &inAction,
                                       const std::string &inContext,
                                       const json &inPayload,
                                       const std::string &inDeviceID)
{
    const std::string event = EPLJSONUtils::GetStringByName(inPayload, "event");

    if (event == "SettingsUpdate") {
        // Update settings for the specified context -- triggered by Property Inspector detecting a change.
        mVisibleContextsMutex.lock();
        if (mVisibleContexts.count(inContext) > 0) {
            mVisibleContexts[inContext]->updateContextSettings(inPayload["settings"]);
        }
        mVisibleContextsMutex.unlock();
    }

    if (event == "RequestDcsStateUpdate") {
        if (!simulator_interface_) {
            mConnectionManager->SendToPropertyInspector(inAction,
                                                        inContext,
                                                        json({{"event", "DebugDcsGameState"},
                                                              {"current_game_state", ""},
                                                              {"error", "SimulatorInterface not connected"}}));
        } else {
            const std::map<int, std::string> dcs_id_values =
                simulator_interface_.value().debug_get_current_game_state();
            json current_game_state;
            for (const auto &[key, value] : dcs_id_values) {
                current_game_state[std::to_string(key)] = value;
            }
            mConnectionManager->SendToPropertyInspector(
                inAction,
                inContext,
                json({{"event", "DebugDcsGameState"}, {"current_game_state", current_game_state}}));
        }
    }

    if (event == "RequestInstalledModules") {
        const std::string dcs_install_path = EPLJSONUtils::GetStringByName(inPayload, "dcs_install_path");
        const std::string modules_subdir = "/mods/aircraft/";
        const json installed_modules_and_result = get_installed_modules(dcs_install_path, modules_subdir);
        const std::string result = EPLJSONUtils::GetStringByName(installed_modules_and_result, "result");
        if (result != "success") {
            mConnectionManager->LogMessage("Get Installed Modules Failure: " + result);
        }
        mConnectionManager->SendToPropertyInspector(
            inAction,
            inContext,
            json({{"event", "InstalledModules"},
                  {"installed_modules", installed_modules_and_result["installed_modules"]}}));
    }

    if (event == "RequestIdLookup") {
        const std::string dcs_install_path = EPLJSONUtils::GetStringByName(inPayload, "dcs_install_path");
        const std::string module = EPLJSONUtils::GetStringByName(inPayload, "module");
        json clickabledata_and_result = get_clickabledata(dcs_install_path, module, "bin/extract_clickabledata.lua");
        const std::string lua_result = EPLJSONUtils::GetStringByName(clickabledata_and_result, "result");
        if (lua_result != "success") {
            mConnectionManager->LogMessage(module + " Clickabledata Result: " + lua_result);
        }
        mConnectionManager->SendToPropertyInspector(
            inAction,
            inContext,
            json({{"event", "Clickabledata"}, {"clickabledata", clickabledata_and_result["clickabledata_items"]}}));
    }
}
