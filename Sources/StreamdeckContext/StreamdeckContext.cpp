// Copyright 2020 Charles Tytler

#include "pch.h"

#include "StreamdeckContext.h"

#include "../Common/EPLJSONUtils.h"

StreamdeckContext::StreamdeckContext(const std::string &context) : context_{context} {};

StreamdeckContext::StreamdeckContext(const std::string &context, const json &settings) : context_{context}
{
    updateContextSettings(settings);
}

void StreamdeckContext::updateContextState(DcsInterface &dcs_interface, ESDConnectionManager *mConnectionManager)
{

    const auto updated_state = comparison_monitor_.determineContextState(dcs_interface);
    const auto updated_title = title_monitor_.determineTitle(dcs_interface);
    increment_monitor_.update(dcs_interface);

    if (updated_state != current_state_) {
        current_state_ = updated_state;
        mConnectionManager->SetState(current_state_, context_);
    }
    if (updated_title != current_title_) {
        current_title_ = updated_title;
        mConnectionManager->SetTitle(current_title_, context_, kESDSDKTarget_HardwareAndSoftware);
    }

    if (delay_for_force_send_state_) {
        if (delay_for_force_send_state_.value()-- <= 0) {
            mConnectionManager->SetState(current_state_, context_);
            delay_for_force_send_state_.reset();
        }
    }
}

void StreamdeckContext::forceSendState(ESDConnectionManager *mConnectionManager)
{
    mConnectionManager->SetState(current_state_, context_);
}

void StreamdeckContext::forceSendStateAfterDelay(const int delay_count)
{
    delay_for_force_send_state_.emplace(delay_count);
}

void StreamdeckContext::updateContextSettings(const json &settings)
{
    comparison_monitor_.update_settings(settings);
    title_monitor_.update_settings(settings);
    increment_monitor_.update_settings(settings);
}

void StreamdeckContext::handleButtonEvent(DcsInterface &dcs_interface,
                                          const KeyEvent event,
                                          const std::string &action,
                                          const json &inPayload)
{
    const std::string button_id = EPLJSONUtils::GetStringByName(inPayload["settings"], "button_id");
    const std::string device_id = EPLJSONUtils::GetStringByName(inPayload["settings"], "device_id");

    if (is_integer(button_id) && is_integer(device_id)) {
        bool send_command = false;
        std::string value = "";
        if (action.find("switch") != std::string::npos) {
            const int state = EPLJSONUtils::GetIntByName(inPayload, "state");
            send_command = determineSendValueForSwitch(event, state, inPayload["settings"], value);
        } else if (action.find("increment") != std::string::npos) {
            send_command = determineSendValueForIncrement(event, inPayload["settings"], value);
        } else {
            send_command = determineSendValueForMomentary(event, inPayload["settings"], value);
        }

        if (send_command) {
            dcs_interface.send_dcs_command(std::stoi(button_id), device_id, value);
        }
    }
}

// TODO: Change determineSend... functions to return an optional rather than a mutable value argument.
bool StreamdeckContext::determineSendValueForMomentary(const KeyEvent event, const json &settings, std::string &value)
{
    if (event == KeyEvent::PRESSED) {
        value = EPLJSONUtils::GetStringByName(settings, "press_value");
    } else {
        if (!EPLJSONUtils::GetBoolByName(settings, "disable_release_check")) {
            value = EPLJSONUtils::GetStringByName(settings, "release_value");
        }
    }
    const bool is_valid = value.empty() ? false : true;
    return is_valid;
}

bool StreamdeckContext::determineSendValueForSwitch(const KeyEvent event,
                                                    const int state,
                                                    const json &settings,
                                                    std::string &value)
{
    if (event == KeyEvent::RELEASED) {
        if (state == 0) {
            value = EPLJSONUtils::GetStringByName(settings, "send_when_first_state_value");
        } else {
            value = EPLJSONUtils::GetStringByName(settings, "send_when_second_state_value");
        }

        if (!value.empty()) {
            return true;
        }
    }
    // Switch type only needs to send command on key down.
    return false;
}

bool StreamdeckContext::determineSendValueForIncrement(const KeyEvent event, const json &settings, std::string &value)
{
    if (event == KeyEvent::PRESSED) {
        const std::string increment_cmd_value_str = EPLJSONUtils::GetStringByName(settings, "increment_value");
        const std::string increment_min_str = EPLJSONUtils::GetStringByName(settings, "increment_min");
        const std::string increment_max_str = EPLJSONUtils::GetStringByName(settings, "increment_max");
        const bool cycling_is_allowed = EPLJSONUtils::GetBoolByName(settings, "increment_cycle_allowed_check");

        if (is_number(increment_cmd_value_str) && is_number(increment_min_str) && is_number(increment_max_str)) {
            const auto current_value = increment_monitor_.get_increment_after_command(Decimal(increment_cmd_value_str),
                                                                                      Decimal(increment_min_str),
                                                                                      Decimal(increment_max_str),
                                                                                      cycling_is_allowed);
            value = current_value.str();
            return true;
        }
    }
    // Increment type only needs to send command on key down.
    return false;
}
