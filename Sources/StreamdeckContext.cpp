// Copyright 2020 Charles Tytler

#include "pch.h"

#include "StreamdeckContext.h"

#include "Common/EPLJSONUtils.h"
#include "Utilities.h"

StreamdeckContext::StreamdeckContext(const std::string &context) { context_ = context; }

StreamdeckContext::StreamdeckContext(const std::string &context, const json &settings) {
    context_ = context;
    updateContextSettings(settings);
}

void StreamdeckContext::updateContextState(DcsInterface &dcs_interface, ESDConnectionManager *mConnectionManager) {
    // Default button states:
    int state = 0;
    std::string title = "";

    if (compare_monitor_is_set_) {
        const std::string current_game_value_raw = dcs_interface.get_value_of_dcs_id(dcs_id_compare_monitor_);
        if (is_number(current_game_value_raw)) {
            float current_game_value = std::stof(current_game_value_raw);
            bool set_context_state_to_second = false;
            switch (dcs_id_compare_condition_) {
            case EQUAL_TO:
                set_context_state_to_second = (current_game_value == dcs_id_comparison_value_);
                break;
            case LESS_THAN:
                set_context_state_to_second = (current_game_value < dcs_id_comparison_value_);
                break;
            case GREATER_THAN:
                set_context_state_to_second = (current_game_value > dcs_id_comparison_value_);
                break;
            }

            state = set_context_state_to_second ? 1 : 0;
        }
    }

    if (string_monitor_is_set_) {
        const std::string current_game_string_value = dcs_interface.get_value_of_dcs_id(dcs_id_string_monitor_);
        if (!current_game_string_value.empty()) {
            title = current_game_string_value;
        }
    }

    if (state != current_state_) {
        current_state_ = state;
        mConnectionManager->SetState(current_state_, context_);
    }
    if (title != current_title_) {
        current_title_ = title;
        mConnectionManager->SetTitle(current_title_, context_, kESDSDKTarget_HardwareAndSoftware);
    }
}

void StreamdeckContext::forceSendState(ESDConnectionManager *mConnectionManager) {
    mConnectionManager->SetState(current_state_, context_);
}

void StreamdeckContext::updateContextSettings(const json &settings) {
    // TODO: Allow comparsion of strings in addition to numeric values.

    const std::string dcs_id_compare_monitor_raw = EPLJSONUtils::GetStringByName(settings, "dcs_id_compare_monitor");
    const std::string dcs_id_compare_condition_raw =
        EPLJSONUtils::GetStringByName(settings, "dcs_id_compare_condition");
    const std::string dcs_id_comparison_value_raw = EPLJSONUtils::GetStringByName(settings, "dcs_id_comparison_value");
    const std::string dcs_id_string_monitor_raw = EPLJSONUtils::GetStringByName(settings, "dcs_id_string_monitor");

    const bool compare_monitor_is_populated = is_integer(dcs_id_compare_monitor_raw);
    const bool comparison_value_is_populated = is_number(dcs_id_comparison_value_raw);
    compare_monitor_is_set_ = compare_monitor_is_populated && comparison_value_is_populated;
    string_monitor_is_set_ = is_integer(dcs_id_string_monitor_raw);

    if (compare_monitor_is_set_) {
        dcs_id_compare_monitor_ = std::stoi(dcs_id_compare_monitor_raw);
        dcs_id_comparison_value_ = std::stof(dcs_id_comparison_value_raw);
        if (dcs_id_compare_condition_raw == "EQUAL_TO") {
            dcs_id_compare_condition_ = EQUAL_TO;
        } else if (dcs_id_compare_condition_raw == "LESS_THAN") {
            dcs_id_compare_condition_ = LESS_THAN;
        } else // Default in Property Inspector html is GREATER_THAN.
        {
            dcs_id_compare_condition_ = GREATER_THAN;
        }
    }

    if (string_monitor_is_set_) {
        dcs_id_string_monitor_ = std::stoi(dcs_id_string_monitor_raw);
    }

    // Set boolean from checkbox using default false value if it doesn't exist in "settings".
    string_monitor_passthrough_ = EPLJSONUtils::GetBoolByName(settings, "string_monitor_passthrough_check", true);
}

void StreamdeckContext::handleButtonEvent(DcsInterface &dcs_interface,
                                          const KeyEvent event,
                                          const std::string &action,
                                          const json &inPayload) {
    const std::string button_id = EPLJSONUtils::GetStringByName(inPayload["settings"], "button_id");
    const std::string device_id = EPLJSONUtils::GetStringByName(inPayload["settings"], "device_id");

    // Set boolean from checkbox using default false value if it doesn't exist in "settings".
    cycle_increments_is_allowed_ =
        EPLJSONUtils::GetBoolByName(inPayload["settings"], "increment_cycle_allowed_check", false);

    if (is_integer(button_id) && is_integer(device_id)) {
        bool send_command = false;
        std::string value = "";
        if (action.find("switch") != std::string::npos) {
            const ContextState state = EPLJSONUtils::GetIntByName(inPayload, "state") == 0 ? FIRST : SECOND;
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

bool StreamdeckContext::determineSendValueForMomentary(const KeyEvent event, const json &settings, std::string &value) {
    if (event == KEY_DOWN) {
        value = EPLJSONUtils::GetStringByName(settings, "press_value");
    } else {
        value = EPLJSONUtils::GetStringByName(settings, "release_value");
    }
    const bool is_valid = value.empty() ? false : true;
    return is_valid;
}

bool StreamdeckContext::determineSendValueForSwitch(const KeyEvent event,
                                                    const ContextState state,
                                                    const json &settings,
                                                    std::string &value) {
    if (event == KEY_DOWN) {
        if (state == FIRST) {
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

bool StreamdeckContext::determineSendValueForIncrement(const KeyEvent event, const json &settings, std::string &value) {
    if (event == KEY_DOWN) {
        const std::string increment_value_str = EPLJSONUtils::GetStringByName(settings, "increment_value");
        const std::string increment_min_str = EPLJSONUtils::GetStringByName(settings, "increment_min");
        const std::string increment_max_str = EPLJSONUtils::GetStringByName(settings, "increment_max");
        const bool cycling_is_allowed = EPLJSONUtils::GetBoolByName(settings, "increment_cycle_allowed_check");
        if (is_number(increment_value_str) && is_number(increment_min_str) && is_number(increment_max_str)) {
            float increment_min = std::stof(increment_min_str);
            float increment_max = std::stof(increment_max_str);

            current_increment_value_ += std::stof(increment_value_str);

            if (current_increment_value_ < increment_min) {
                current_increment_value_ = cycle_increments_is_allowed_ ? increment_max : increment_min;
            } else if (current_increment_value_ > increment_max) {
                current_increment_value_ = cycle_increments_is_allowed_ ? increment_min : increment_max;
            }
            value = std::to_string(current_increment_value_);
            return true;
        }
    }
    // Increment type only needs to send command on key down.
    return false;
}
