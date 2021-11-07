// Copyright 2021 Charles Tytler

#include "SwitchContext.h"

#include "ElgatoSD/EPLJSONUtils.h"

void SwitchContext::handleButtonPressedEvent(const std::unique_ptr<SimulatorInterface> &simulator_interface,
                                             ESDConnectionManager *mConnectionManager,
                                             const json &inPayload)
{
    // Nothing sent to DCS on press.
}

void SwitchContext::handleButtonReleasedEvent(const std::unique_ptr<SimulatorInterface> &simulator_interface,
                                              ESDConnectionManager *mConnectionManager,
                                              const json &inPayload)
{
    const auto button_id = EPLJSONUtils::GetStringByName(inPayload["settings"], "button_id");
    const auto device_id = EPLJSONUtils::GetStringByName(inPayload["settings"], "device_id");
    const auto address = device_id + "," + button_id;
    const auto send_when_first_state_value =
        EPLJSONUtils::GetStringByName(inPayload["settings"], "send_when_first_state_value");
    const auto send_when_second_state_value =
        EPLJSONUtils::GetStringByName(inPayload["settings"], "send_when_second_state_value");
    const bool is_first_state = (EPLJSONUtils::GetIntByName(inPayload, "state") == 0);

    if (!send_when_first_state_value.empty() && !send_when_second_state_value.empty()) {
        const auto send_value = is_first_state ? send_when_first_state_value : send_when_second_state_value;
        simulator_interface->send_simulator_command(address, send_value);
    }

    // The Streamdeck will by default change a context's state after a KeyUp event, so a force send of the current
    // context's state will keep the button state in sync with the plugin.

    // For switches use a delay to avoid jittering and a race condition of Streamdeck and Plugin trying to
    // change state.
    forceSendStateAfterDelay(num_frames_delay_forced_state_update_);
}
