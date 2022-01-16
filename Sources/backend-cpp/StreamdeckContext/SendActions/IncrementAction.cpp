// Copyright 2021 Charles Tytler

#include "IncrementAction.h"

#include "Utilities/StringUtilities.h"

#include "ElgatoSD/EPLJSONUtils.h"

void IncrementAction::handleButtonPressedEvent(const std::unique_ptr<SimulatorInterface> &simulator_interface,
                                               ESDConnectionManager *mConnectionManager,
                                               const json &inPayload)
{
    const auto settings = inPayload["settings"];
    const auto send_address = EPLJSONUtils::GetStringByName(settings, "send_address");

    // TODO: simplify increment monitor interface as this is currently the only user.
    increment_monitor_.update_settings(settings);
    increment_monitor_.update(simulator_interface);

    const auto send_command = determineSendValue(settings);
    if (send_command) {
        simulator_interface->send_command(send_address, send_command.value());
    }
}

void IncrementAction::handleButtonReleasedEvent(const std::unique_ptr<SimulatorInterface> &simulator_interface,
                                                ESDConnectionManager *mConnectionManager,
                                                const json &inPayload)
{
    // Nothing sent to DCS on release.
}

std::optional<std::string> IncrementAction::determineSendValue(const json &settings)
{
    const auto increment_cmd_value_str = EPLJSONUtils::GetStringByName(settings, "increment_value");
    const auto increment_min_str = EPLJSONUtils::GetStringByName(settings, "increment_min");
    const auto increment_max_str = EPLJSONUtils::GetStringByName(settings, "increment_max");
    const bool cycling_is_allowed = EPLJSONUtils::GetBoolByName(settings, "increment_cycle_allowed_check");

    if (is_number(increment_cmd_value_str) && is_number(increment_min_str) && is_number(increment_max_str)) {
        const auto value = increment_monitor_.get_increment_after_command(Decimal(increment_cmd_value_str),
                                                                          Decimal(increment_min_str),
                                                                          Decimal(increment_max_str),
                                                                          cycling_is_allowed);
        return value.str();
    }
    return std::nullopt;
}
