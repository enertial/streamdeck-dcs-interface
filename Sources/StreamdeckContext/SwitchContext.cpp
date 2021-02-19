// Copyright 2021 Charles Tytler

#include "pch.h"

#include "SwitchContext.h"

#include "../Common/EPLJSONUtils.h"

SwitchContext::SwitchContext(const std::string &context) { context_ = context; }

SwitchContext::SwitchContext(const std::string &context, const json &settings)
{
    context_ = context;
    updateContextSettings(settings);
}

void SwitchContext::handleButtonEvent(DcsInterface *dcs_interface, const KeyEvent event, const json &inPayload)
{
    const std::string button_id = EPLJSONUtils::GetStringByName(inPayload["settings"], "button_id");
    const std::string device_id = EPLJSONUtils::GetStringByName(inPayload["settings"], "device_id");

    if (is_integer(button_id) && is_integer(device_id)) {
        bool send_command = false;
        std::string value = "";

        const ContextState state = EPLJSONUtils::GetIntByName(inPayload, "state") == 0 ? FIRST : SECOND;
        send_command = determineSendValue(event, state, inPayload["settings"], value);

        if (send_command) {
            dcs_interface->send_dcs_command(std::stoi(button_id), device_id, value);
        }
    }
}

bool SwitchContext::determineSendValue(const KeyEvent event,
                                       const ContextState state,
                                       const json &settings,
                                       std::string &value)
{
    if (event == KeyEvent::RELEASED) {
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
