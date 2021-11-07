// Copyright 2021 Charles Tytler

#include "MomentaryContext.h"

#include "ElgatoSD/EPLJSONUtils.h"

void MomentaryContext::handleButtonPressedEvent(SimulatorInterface &simulator_interface,
                                                ESDConnectionManager *mConnectionManager,
                                                const json &inPayload)
{
    const auto button_id = EPLJSONUtils::GetStringByName(inPayload["settings"], "button_id");
    const auto device_id = EPLJSONUtils::GetStringByName(inPayload["settings"], "device_id");
    const auto send_command = EPLJSONUtils::GetStringByName(inPayload["settings"], "press_value");

    if (is_integer(button_id) && is_integer(device_id) && !send_command.empty()) {
        simulator_interface.send_simulator_command(std::stoi(button_id), device_id, send_command);
    }
}

void MomentaryContext::handleButtonReleasedEvent(SimulatorInterface &simulator_interface,
                                                 ESDConnectionManager *mConnectionManager,
                                                 const json &inPayload)
{
    // The Streamdeck will by default change a context's state after a KeyUp event, so a force send of the current
    // context's state will keep the button state in sync with the plugin.
    forceSendState(mConnectionManager);

    const auto button_id = EPLJSONUtils::GetStringByName(inPayload["settings"], "button_id");
    const auto device_id = EPLJSONUtils::GetStringByName(inPayload["settings"], "device_id");
    const auto send_command = EPLJSONUtils::GetStringByName(inPayload["settings"], "release_value");
    const bool send_on_release_is_disabled =
        EPLJSONUtils::GetBoolByName(inPayload["settings"], "disable_release_check");

    if (is_integer(button_id) && is_integer(device_id) && !send_command.empty() && !send_on_release_is_disabled) {
        simulator_interface.send_simulator_command(std::stoi(button_id), device_id, send_command);
    }
}
