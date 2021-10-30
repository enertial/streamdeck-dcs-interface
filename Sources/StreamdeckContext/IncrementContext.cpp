// Copyright 2021 Charles Tytler

#include "pch.h"

#include "IncrementContext.h"

#include "../Common/EPLJSONUtils.h"

IncrementContext::IncrementContext(const std::string &context) { context_ = context; }

IncrementContext::IncrementContext(const std::string &context, const json &settings)
{
    context_ = context;
    updateContextSettings(settings);
}

void IncrementContext::handleButtonEvent(DcsInterface *dcs_interface, const KeyEvent event, const json &inPayload)
{
    const std::string button_id = EPLJSONUtils::GetStringByName(inPayload["settings"], "button_id");
    const std::string device_id = EPLJSONUtils::GetStringByName(inPayload["settings"], "device_id");

    // Set boolean from checkbox using default false value if it doesn't exist in "settings".
    cycle_increments_is_allowed_ =
        EPLJSONUtils::GetBoolByName(inPayload["settings"], "increment_cycle_allowed_check", false);

    if (is_integer(button_id) && is_integer(device_id)) {
        const auto send_command = determineSendValue(event, inPayload["settings"]);

        if (send_command) {
            dcs_interface->send_dcs_command(std::stoi(button_id), device_id, send_command.value());
        }
    }
}

std::optional<std::string> IncrementContext::determineSendValue(const KeyEvent event, const json &settings) const
{
    if (event == KeyEvent::PRESSED) {
        const std::string increment_value_str = EPLJSONUtils::GetStringByName(settings, "increment_value");
        const std::string increment_min_str = EPLJSONUtils::GetStringByName(settings, "increment_min");
        const std::string increment_max_str = EPLJSONUtils::GetStringByName(settings, "increment_max");
        const bool cycling_is_allowed = EPLJSONUtils::GetBoolByName(settings, "increment_cycle_allowed_check");
        if (is_number(increment_value_str) && is_number(increment_min_str) && is_number(increment_max_str)) {
            Decimal increment_min(increment_min_str);
            Decimal increment_max(increment_max_str);

            current_increment_value_ += Decimal(increment_value_str);

            if (current_increment_value_ < increment_min) {
                current_increment_value_ = cycle_increments_is_allowed_ ? increment_max : increment_min;
            } else if (current_increment_value_ > increment_max) {
                current_increment_value_ = cycle_increments_is_allowed_ ? increment_min : increment_max;
            }
            return current_increment_value_.str();
        }
    }
    // Increment type only needs to send command on key down.
    return std::nullopt;
}