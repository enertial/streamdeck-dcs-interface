// Copyright 2021 Charles Tytler

#include "MomentaryAction.h"

#include "ElgatoSD/EPLJSONUtils.h"

void MomentaryAction::handleButtonPressedEvent(SimulatorInterface *simulator_interface,
                                               ESDConnectionManager *mConnectionManager,
                                               const json &inPayload)
{
    const auto send_address = EPLJSONUtils::GetStringByName(inPayload["settings"], "send_address");
    const auto send_command = EPLJSONUtils::GetStringByName(inPayload["settings"], "press_value");

    if (!send_command.empty()) {
        simulator_interface->send_command(send_address, send_command);
    }
}

void MomentaryAction::handleButtonReleasedEvent(SimulatorInterface *simulator_interface,
                                                ESDConnectionManager *mConnectionManager,
                                                const json &inPayload)
{
    const auto send_address = EPLJSONUtils::GetStringByName(inPayload["settings"], "send_address");
    const auto send_command = EPLJSONUtils::GetStringByName(inPayload["settings"], "release_value");
    const bool send_on_release_is_disabled =
        EPLJSONUtils::GetBoolByName(inPayload["settings"], "disable_release_check");

    if (!send_command.empty() && !send_on_release_is_disabled) {
        simulator_interface->send_command(send_address, send_command);
    }
}
