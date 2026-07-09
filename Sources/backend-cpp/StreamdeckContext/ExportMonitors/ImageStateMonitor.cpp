// Copyright 2021 Charles Tytler

#include "ImageStateMonitor.h"

#include "Utilities/StringUtilities.h"

ImageStateMonitor::ImageStateMonitor(const json &settings) { update_settings(settings); }

void ImageStateMonitor::update_settings(const json &state_monitor_settings)
{
    settings_per_state_.clear();
    monitor_is_set_ = state_monitor_settings["is_set"];

    if (monitor_is_set_) {
        for (const auto &state : state_monitor_settings) {
            SimulatorAddress address;
            if (state["monitor_address"]["type"] == "INTEGER") {
                address = SimulatorAddress{state["monitor_address"]["address"],
                                           state["monitor_address"]["mask"],
                                           state["monitor_address"]["shift"]};
            } else if (state["monitor_address"]["type"] == "ADDRESS_ONLY") {
                address = SimulatorAddress{state["monitor_address"]["address"]};
            } else {
                // Unsupported type.
                break;
            }

            Comparison comp_type;
            if (state["comparison_type"] == "LT") {
                comp_type = Comparison::LESS_THAN;
            } else if (state["comparison_type"] == "GT") {
                comp_type = Comparison::GREATER_THAN;
            } else {
                comp_type = Comparison::EQUAL_TO;
            }
            settings_per_state_.push_back(StateMonitorSettings{address, comp_type, Decimal(state["comparison_value"])});
        }
    }
}

int ImageStateMonitor::determineContextState(SimulatorInterface *simulator_interface) const
{
    if (monitor_is_set_) {
        for (int iState = 0; iState < settings_per_state_.size(); ++iState) {
            const auto maybe_value = simulator_interface->get_value_at_addr(settings_per_state_[iState].address);
            if (!maybe_value) {
                return 0; // Return early if not all addresses are readable.
            }
            if (maybe_value && comparison_is_satisfied(maybe_value.value())) {
                return iState;
            }
        }
        // If no conditional states are satisfied, then the last state is set.
        return settings_per_state_.size() + 1;
    }
    // Default to first state.
    return 0;
}

bool ImageStateMonitor::comparison_is_satisfied(const Decimal &current_game_value,
                                                const Comparison &comparison,
                                                const Decimal &comparison_value) const
{
    bool comparison_result = false;
    switch (comparison) {
    case Comparison::EQUAL_TO:
        comparison_result = (current_game_value == comparison_value);
        break;
    case Comparison::LESS_THAN:
        comparison_result = (current_game_value < comparison_value);
        break;
    case Comparison::GREATER_THAN:
        comparison_result = (current_game_value > comparison_value);
        break;
    }
    return comparison_result;
}
