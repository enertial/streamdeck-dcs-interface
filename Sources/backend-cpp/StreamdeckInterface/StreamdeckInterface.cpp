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
#include "SimulatorInterface/SimulatorInterfaceParameters.h"
#include "StreamdeckContext/BackwardsCompatibilityHandler.h"
#include "Utilities/JsonReader.h"
#include "Utilities/LuaReader.h"

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
    // SimulatorConnectionSettings connection_settings = get_connection_settings(settings);

    // TODO: read these in from global settings, always connect to both for now.
    std::unordered_map<Protocol, SimulatorConnectionSettings> connection_settings;
    connection_settings[Protocol::DCS_BIOS] = {"5010", "7778", "127.0.0.1", "239.255.50.10"};
    connection_settings[Protocol::DCS_ExportScript] = {"1725", "26027", "127.0.0.1", ""};

    // Create first instance of Simulator Interface only done under DidReceiveGlobalSettings to allow for any stored
    // settings to be used before binding socket to default port values.
    for (const auto &protocol : connection_settings) {
        if (!simConnectionManager_.is_connected_with_settings(protocol.first, protocol.second)) {
            try {
                simConnectionManager_.connect_to_protocol(protocol.first, protocol.second);
                mConnectionManager->LogMessage("Successfully connected to Simulator Interface UDP port");
            } catch (const std::exception &e) {
                mConnectionManager->LogMessage("Caught Exception While Opening Connection: " + std::string(e.what()));
            }
        }
    }
}

void StreamdeckInterface::UpdateFromGameState()
{
    //
    // Warning: UpdateFromGameState() is running in the timer thread
    //

    // Update the Simulator game state in memory, then update each Streamdeck button context.
    simConnectionManager_.update_all();

    if (mConnectionManager != nullptr) {
        mVisibleContextsMutex.lock();
        for (auto &context : mVisibleContexts) {
            const auto protocol = context.second.protocol();
            if (simConnectionManager_.is_connected(protocol)) {
                context.second.updateContextState(simConnectionManager_.get_interface(protocol), mConnectionManager);
            }
        }
        mVisibleContextsMutex.unlock();
    }
}

void StreamdeckInterface::KeyDownForAction(const std::string &inAction,
                                           const std::string &inContext,
                                           const json &inPayload,
                                           const std::string &inDeviceID)
{
    const auto payload = backwardsCompatibilityHandler(inPayload);

    mVisibleContextsMutex.lock();
    const auto protocol = mVisibleContexts[inContext].protocol();
    if (simConnectionManager_.is_connected(protocol)) {
        mVisibleContexts[inContext].handleButtonPressedEvent(
            simConnectionManager_.get_interface(protocol), mConnectionManager, payload);
    }
    mVisibleContextsMutex.unlock();
}

void StreamdeckInterface::KeyUpForAction(const std::string &inAction,
                                         const std::string &inContext,
                                         const json &inPayload,
                                         const std::string &inDeviceID)
{
    const auto payload = backwardsCompatibilityHandler(inPayload);

    mVisibleContextsMutex.lock();
    const auto protocol = mVisibleContexts[inContext].protocol();
    if (simConnectionManager_.is_connected(protocol)) {
        mVisibleContexts[inContext].handleButtonReleasedEvent(
            simConnectionManager_.get_interface(protocol), mConnectionManager, payload);
    }
    mVisibleContextsMutex.unlock();
}

void StreamdeckInterface::WillAppearForAction(const std::string &inAction,
                                              const std::string &inContext,
                                              const json &inPayload,
                                              const std::string &inDeviceID)
{
    const auto settings = inPayload["settings"];
    auto newContext = StreamdeckContext(inAction, inContext, settings);

    if (newContext.is_valid()) {
        mVisibleContextsMutex.lock();
        // Remember the context and make sure state is synchronized with plugin.
        mVisibleContexts[inContext] = std::move(newContext);
        mVisibleContexts[inContext].forceSendState(mConnectionManager);
        mVisibleContextsMutex.unlock();
    } else {
        mConnectionManager->LogMessage("Unable to handle button of type: " + inAction + " context: " + inContext +
                                       " with Settings: " + settings.dump());
    }
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
            mVisibleContexts[inContext].updateContextSettings(inPayload["settings"]);
        }
        mVisibleContextsMutex.unlock();
    }

    if (event == "RequestDcsStateUpdate") {
        const auto protocol =
            (inAction == "com.ctytler.dcs.dcs-bios") ? Protocol::DCS_BIOS : Protocol::DCS_ExportScript;
        if (!simConnectionManager_.is_connected(protocol)) {
            mConnectionManager->SendToPropertyInspector(inAction,
                                                        inContext,
                                                        json({{"event", "DebugDcsGameState"},
                                                              {"current_game_state", ""},
                                                              {"error", "SimulatorInterface not connected"}}));
        } else {
            const json current_simulator_state =
                simConnectionManager_.get_interface(protocol)->get_current_state_as_json();
            mConnectionManager->SendToPropertyInspector(
                inAction,
                inContext,
                json({{"event", "DebugDcsGameState"}, {"current_game_state", current_simulator_state}}));
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

    if (event == "requestModuleList") {
        const std::string path = EPLJSONUtils::GetStringByName(inPayload, "path");
        const auto maybe_module_list = get_module_list(path);
        if (maybe_module_list) {
            mConnectionManager->SendToPropertyInspector(
                inAction, inContext, json({{"event", "ModuleList"}, {"moduleList", maybe_module_list.value()}}));
            mConnectionManager->LogMessage("Successfully found json modules at: " + path);
        } else {
            mConnectionManager->LogMessage("Get list of json modules failed at: " + path);
        }
    }

    if (event == "requestControlReferenceJson") {
        const std::string filename = EPLJSONUtils::GetStringByName(inPayload, "filename");
        const auto maybe_json = read_json_file(filename);
        if (maybe_json) {
            mConnectionManager->SendToPropertyInspector(
                inAction, inContext, json({{"event", "JsonFile"}, {"jsonFile", maybe_json.value()}}));
            mConnectionManager->LogMessage("Successfully read in json file from: " + filename);
        } else {
            mConnectionManager->LogMessage("Unable to read in json file from: " + filename);
        }
    }
}
