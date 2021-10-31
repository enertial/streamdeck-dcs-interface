// Copyright 2021 Charles Tytler

#include "pch.h"

#include "SwitchContext.h"

#include "../../Common/EPLJSONUtils.h"

void SwitchContext::handleButtonEvent(DcsInterface &dcs_interface, const KeyEvent event, const json &inPayload)
{
    const std::string button_id = EPLJSONUtils::GetStringByName(inPayload["settings"], "button_id");
    const std::string device_id = EPLJSONUtils::GetStringByName(inPayload["settings"], "device_id");

    if (is_integer(button_id) && is_integer(device_id)) {
        const ContextState state =
            EPLJSONUtils::GetIntByName(inPayload, "state") == 0 ? ContextState::FIRST : ContextState::SECOND;
        const auto send_command = determineSendValue(event, state, inPayload["settings"]);

        if (send_command) {
            dcs_interface.send_dcs_command(std::stoi(button_id), device_id, send_command.value());
        }
    }
}

std::optional<std::string>
SwitchContext::determineSendValue(const KeyEvent event, const ContextState state, const json &settings) const
{
    std::string value;
    if (event == KeyEvent::RELEASED) {
        if (state == ContextState::FIRST) {
            value = EPLJSONUtils::GetStringByName(settings, "send_when_first_state_value");
        } else {
            value = EPLJSONUtils::GetStringByName(settings, "send_when_second_state_value");
        }

        if (!value.empty()) {
            return value;
        }
    }
    // Switch type only needs to send command on key down.
    return std::nullopt;
}
