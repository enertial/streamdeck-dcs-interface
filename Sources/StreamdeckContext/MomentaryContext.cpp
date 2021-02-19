// Copyright 2021 Charles Tytler

#include "pch.h"

#include "MomentaryContext.h"

#include "../Common/EPLJSONUtils.h"

MomentaryContext::MomentaryContext(const std::string &context) { context_ = context; }

MomentaryContext::MomentaryContext(const std::string &context, const json &settings)
{
    context_ = context;
    updateContextSettings(settings);
}

void MomentaryContext::handleButtonEvent(DcsInterface *dcs_interface, const KeyEvent event, const json &inPayload)
{
    const std::string button_id = EPLJSONUtils::GetStringByName(inPayload["settings"], "button_id");
    const std::string device_id = EPLJSONUtils::GetStringByName(inPayload["settings"], "device_id");

    if (is_integer(button_id) && is_integer(device_id)) {
        bool send_command = false;
        std::string value = "";

        send_command = determineSendValue(event, inPayload["settings"], value);

        if (send_command) {
            dcs_interface->send_dcs_command(std::stoi(button_id), device_id, value);
        }
    }
}

bool MomentaryContext::determineSendValue(const KeyEvent event, const json &settings, std::string &value)
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