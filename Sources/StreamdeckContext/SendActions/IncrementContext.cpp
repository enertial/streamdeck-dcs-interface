// Copyright 2021 Charles Tytler

#include "IncrementContext.h"

#include "ElgatoSD/EPLJSONUtils.h"

void IncrementContext::handleButtonEvent(DcsInterface &dcs_interface, const KeyEvent event, const json &inPayload)
{
    const std::string button_id = EPLJSONUtils::GetStringByName(inPayload["settings"], "button_id");
    const std::string device_id = EPLJSONUtils::GetStringByName(inPayload["settings"], "device_id");

    if (is_integer(button_id) && is_integer(device_id)) {
        const auto send_command = determineSendValue(event, inPayload["settings"]);

        if (send_command) {
            dcs_interface.send_dcs_command(std::stoi(button_id), device_id, send_command.value());
        }
    }
}

std::optional<std::string> IncrementContext::determineSendValue(const KeyEvent event, const json &settings)
{
    if (event == KeyEvent::PRESSED) {
        const std::string increment_cmd_value_str = EPLJSONUtils::GetStringByName(settings, "increment_value");
        const std::string increment_min_str = EPLJSONUtils::GetStringByName(settings, "increment_min");
        const std::string increment_max_str = EPLJSONUtils::GetStringByName(settings, "increment_max");
        const bool cycling_is_allowed = EPLJSONUtils::GetBoolByName(settings, "increment_cycle_allowed_check");

        if (is_number(increment_cmd_value_str) && is_number(increment_min_str) && is_number(increment_max_str)) {
            const auto value = increment_monitor_.get_increment_after_command(Decimal(increment_cmd_value_str),
                                                                              Decimal(increment_min_str),
                                                                              Decimal(increment_max_str),
                                                                              cycling_is_allowed);
            return value.str();
        }
    }
    // Increment type only needs to send command on key down.
    return std::nullopt;
}