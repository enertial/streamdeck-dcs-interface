//==============================================================================
/**
@file       MyStreamDeckPlugin.cpp

@brief      Based off of Elgato's CPU plugin example

@copyright  (c) 2018, Corsair Memory, Inc.
                        This source code is licensed under the MIT-style license found in the LICENSE file.

**/
//==============================================================================

#include "MyStreamDeckPlugin.h"
#include <atomic>

#include "Common/EPLJSONUtils.h"
#include "Common/ESDConnectionManager.h"
#include "DcsInterfaceParameters.h"

class CallBackTimer {
  public:
    CallBackTimer() : _execute(false) {}

    ~CallBackTimer() {
        if (_execute.load(std::memory_order_acquire)) {
            stop();
        };
    }

    void stop() {
        _execute.store(false, std::memory_order_release);
        if (_thd.joinable())
            _thd.join();
    }

    void start(int interval, std::function<void(void)> func) {
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

MyStreamDeckPlugin::MyStreamDeckPlugin() : dcs_interface_(kDcsListenerPort, kDcsSendPort, kDcsIpAddress) {
    mTimer = new CallBackTimer();
    mTimer->start(10, [this]() { this->UpdateFromGameState(); });
    // Send a reset to command in case DCS mission is already running.
    dcs_interface_.send_dcs_reset_command();
}

MyStreamDeckPlugin::~MyStreamDeckPlugin() {
    if (mTimer != nullptr) {
        mTimer->stop();

        delete mTimer;
        mTimer = nullptr;
    }
}

void MyStreamDeckPlugin::UpdateFromGameState() {
    //
    // Warning: UpdateFromGameState() is running in the timer thread
    //

    // Update the DCS game state in memory, then update each Streamdeck button context.
    dcs_interface_.update_dcs_state();

    if (mConnectionManager != nullptr) {
        mVisibleContextsMutex.lock();
        for (auto &elem : mVisibleContexts) {
            elem.second.updateContextState(dcs_interface_, mConnectionManager);
        }
        mVisibleContextsMutex.unlock();
    }
}

void MyStreamDeckPlugin::KeyDownForAction(const std::string &inAction,
                                          const std::string &inContext,
                                          const json &inPayload,
                                          const std::string &inDeviceID) {
    // Send a command to DCS using the settings included with the KeyDownForAction callback.
    json settings;
    EPLJSONUtils::GetObjectByName(inPayload, "settings", settings);
    const std::string button_id = EPLJSONUtils::GetStringByName(settings, "button_id");
    const std::string device_id = EPLJSONUtils::GetStringByName(settings, "device_id");

    // For switches the state of the context must be used:
    //      release_value used in state 1, press_value used in state 2.
    std::string value = "";
    const bool is_switch_action = (inAction.find("switch") != std::string::npos);
    const bool is_increment_action = (inAction.find("increment") != std::string::npos);
    if (is_switch_action) {
        const int state = EPLJSONUtils::GetIntByName(inPayload, "state");
        if (state == 0) {
            value = EPLJSONUtils::GetStringByName(settings, "send_when_first_state_value");
        } else {
            value = EPLJSONUtils::GetStringByName(settings, "send_when_second_state_value");
        }
    } else if (is_increment_action) {
        value = EPLJSONUtils::GetStringByName(settings, "increment_value");
        // TODO: handle incrementing value.

    } else {
        value = EPLJSONUtils::GetStringByName(settings, "press_value");

        // The Streamdeck will by default change a context's state after a button action, so a force send of the
        // current context's state will keep the button state in sync with the plugin. (Not performed for switches
        // as generally the change in state is desired there).
        mVisibleContexts[inContext].forceSendState(mConnectionManager);
    }

    if (!button_id.empty() && !device_id.empty() && !value.empty()) {
        dcs_interface_.send_dcs_command(std::stoi(button_id), device_id, value);
    }
}

void MyStreamDeckPlugin::KeyUpForAction(const std::string &inAction,
                                        const std::string &inContext,
                                        const json &inPayload,
                                        const std::string &inDeviceID) {

    // For switches no change of value is needed on key up.
    const bool is_switch_action = (inAction.find("switch") != std::string::npos);
    const bool is_increment_action = (inAction.find("increment") != std::string::npos);
    if (!is_switch_action && !is_increment_action) {
        // Send a command to DCS using the settings included with the KeyUpForAction callback.
        json settings;
        EPLJSONUtils::GetObjectByName(inPayload, "settings", settings);
        const std::string button_id = EPLJSONUtils::GetStringByName(settings, "button_id");
        const std::string device_id = EPLJSONUtils::GetStringByName(settings, "device_id");
        const std::string value = EPLJSONUtils::GetStringByName(settings, "release_value");
        if (!button_id.empty() && !device_id.empty() && !value.empty()) {
            dcs_interface_.send_dcs_command(std::stoi(button_id), device_id, value);
        }
    }

    // The Streamdeck will by default change a context's state after a button action, so a force send of the current
    // context's state will keep the button state in sync with the plugin.
    mVisibleContexts[inContext].forceSendState(mConnectionManager);
}

void MyStreamDeckPlugin::WillAppearForAction(const std::string &inAction,
                                             const std::string &inContext,
                                             const json &inPayload,
                                             const std::string &inDeviceID) {
    // Remember the context.
    mVisibleContextsMutex.lock();
    json settings;
    EPLJSONUtils::GetObjectByName(inPayload, "settings", settings);
    mVisibleContexts[inContext] = StreamdeckContext(inContext, settings);
    mVisibleContexts[inContext].updateContextState(dcs_interface_, mConnectionManager);
    mVisibleContextsMutex.unlock();
}

void MyStreamDeckPlugin::WillDisappearForAction(const std::string &inAction,
                                                const std::string &inContext,
                                                const json &inPayload,
                                                const std::string &inDeviceID) {
    // Remove the context.
    mVisibleContextsMutex.lock();
    mVisibleContexts.erase(inContext);
    mVisibleContextsMutex.unlock();
}

void MyStreamDeckPlugin::DeviceDidConnect(const std::string &inDeviceID, const json &inDeviceInfo) {
    // Nothing to do.
}

void MyStreamDeckPlugin::DeviceDidDisconnect(const std::string &inDeviceID) {
    // Nothing to do.
}

void MyStreamDeckPlugin::SendToPlugin(const std::string &inAction,
                                      const std::string &inContext,
                                      const json &inPayload,
                                      const std::string &inDeviceID) {
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
        const std::map<int, std::string> dcs_id_values = dcs_interface_.debug_get_current_game_state();
        json current_game_state;
        for (const auto &[key, value] : dcs_id_values) {
            current_game_state[std::to_string(key)] = value;
        }
        mConnectionManager->SendToPropertyInspector(
            inAction, inContext, json({{"event", "DebugDcsGameState"}, {"current_game_state", current_game_state}}));
    }
}
