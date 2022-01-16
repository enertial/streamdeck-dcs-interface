// Copyright 2021 Charles Tytler

#include "SwitchAction.h"

#include "ElgatoSD/EPLJSONUtils.h"

void SwitchAction::handleButtonPressedEvent(const std::unique_ptr<SimulatorInterface> &simulator_interface,
                                            ESDConnectionManager *mConnectionManager,
                                            const json &inPayload)
{
    // Nothing sent to DCS on press.
}

void SwitchAction::handleButtonReleasedEvent(const std::unique_ptr<SimulatorInterface> &simulator_interface,
                                             ESDConnectionManager *mConnectionManager,
                                             const json &inPayload)
{
    const auto send_address = EPLJSONUtils::GetStringByName(inPayload["settings"], "send_address");
    const auto send_when_first_state_value =
        EPLJSONUtils::GetStringByName(inPayload["settings"], "send_when_first_state_value");
    const auto send_when_second_state_value =
        EPLJSONUtils::GetStringByName(inPayload["settings"], "send_when_second_state_value");
    const bool is_first_state = (EPLJSONUtils::GetIntByName(inPayload, "state") == 0);

    if (!send_when_first_state_value.empty() && !send_when_second_state_value.empty()) {
        const auto send_value = is_first_state ? send_when_first_state_value : send_when_second_state_value;
        simulator_interface->send_command(send_address, send_value);
    }
}
