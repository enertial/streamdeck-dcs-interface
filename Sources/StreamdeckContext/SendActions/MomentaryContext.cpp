// Copyright 2021 Charles Tytler

#include "MomentaryContext.h"

#include "ElgatoSD/EPLJSONUtils.h"

void MomentaryContext::handleButtonEvent(DcsInterface &dcs_interface, const KeyEvent event, const json &inPayload)
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

std::optional<std::string> MomentaryContext::determineSendValue(const KeyEvent event, const json &settings) const
{
    std::string value;
    if (event == KeyEvent::PRESSED) {
        value = EPLJSONUtils::GetStringByName(settings, "press_value");
    } else {
        if (!EPLJSONUtils::GetBoolByName(settings, "disable_release_check")) {
            value = EPLJSONUtils::GetStringByName(settings, "release_value");
        }
    }

    if (!value.empty()) {
        return value;
    }
    return std::nullopt;
}