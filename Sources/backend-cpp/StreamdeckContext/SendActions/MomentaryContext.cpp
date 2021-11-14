// Copyright 2021 Charles Tytler

#include "MomentaryContext.h"

#include "ElgatoSD/EPLJSONUtils.h"

void MomentaryContext::handleButtonPressedEvent(const std::unique_ptr<SimulatorInterface> &simulator_interface,
                                                ESDConnectionManager *mConnectionManager,
                                                const json &inPayload)
{
    const auto send_address = EPLJSONUtils::GetStringByName(inPayload["settings"], "send_address");
    const auto send_command = EPLJSONUtils::GetStringByName(inPayload["settings"], "press_value");

    if (!send_command.empty()) {
        simulator_interface->send_simulator_command(send_address, send_command);
    }
}

void MomentaryContext::handleButtonReleasedEvent(const std::unique_ptr<SimulatorInterface> &simulator_interface,
                                                 ESDConnectionManager *mConnectionManager,
                                                 const json &inPayload)
{
    // The Streamdeck will by default change a context's state after a KeyUp event, so a force send of the current
    // context's state will keep the button state in sync with the plugin.
    forceSendState(mConnectionManager);

    const auto send_address = EPLJSONUtils::GetStringByName(inPayload["settings"], "send_address");
    const auto send_command = EPLJSONUtils::GetStringByName(inPayload["settings"], "release_value");
    const bool send_on_release_is_disabled =
        EPLJSONUtils::GetBoolByName(inPayload["settings"], "disable_release_check");

    if (!send_command.empty() && !send_on_release_is_disabled) {
        simulator_interface->send_simulator_command(send_address, send_command);
    }
}
