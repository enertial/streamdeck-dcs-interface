// Copyright 2021 Charles Tytler

#include "ImageStateMonitor.h"

#include "Utilities/StringUtilities.h"

ImageStateMonitor::ImageStateMonitor(const json &settings) { update_settings(settings); }

void ImageStateMonitor::update_settings(const json &settings)
{
    const std::string dcs_id_compare_monitor_raw = EPLJSONUtils::GetStringByName(settings, "dcs_id_compare_monitor");
    const std::string dcs_id_compare_condition_raw =
        EPLJSONUtils::GetStringByName(settings, "dcs_id_compare_condition");
    const std::string dcs_id_comparison_value_raw = EPLJSONUtils::GetStringByName(settings, "dcs_id_comparison_value");

    const bool compare_monitor_is_populated = !dcs_id_compare_monitor_raw.empty();
    const bool comparison_value_is_populated = is_number(dcs_id_comparison_value_raw);
    settings_are_filled_ = compare_monitor_is_populated && comparison_value_is_populated;

    if (settings_are_filled_) {
        if (is_integer(dcs_id_compare_monitor_raw)) {
            dcs_id_compare_monitor_ = SimulatorAddress(std::stoi(dcs_id_compare_monitor_raw));

        } else {
            if (dcs_id_compare_monitor_raw == "INTEGER") {
                dcs_id_compare_monitor_ = SimulatorAddress(settings["compare_monitor_address"],
                                                           settings["compare_monitor_mask"],
                                                           settings["compare_monitor_shift"]);
            } else if (dcs_id_compare_monitor_raw == "STRING") {
                dcs_id_compare_monitor_ =
                    SimulatorAddress(settings["compare_monitor_address"], settings["compare_monitor_max_length"]);
            }
        }
        dcs_id_comparison_value_ = Decimal(dcs_id_comparison_value_raw);
        if (dcs_id_compare_condition_raw == "EQUAL_TO") {
            dcs_id_compare_condition_ = Comparison::EQUAL_TO;
        } else if (dcs_id_compare_condition_raw == "LESS_THAN") {
            dcs_id_compare_condition_ = Comparison::LESS_THAN;
        } else // Default in Property Inspector html is GREATER_THAN.
        {
            dcs_id_compare_condition_ = Comparison::GREATER_THAN;
        }
    }
}

int ImageStateMonitor::determineContextState(SimulatorInterface *simulator_interface) const
{
    if (settings_are_filled_) {
        const auto maybe_current_game_value = simulator_interface->get_value_at_addr(dcs_id_compare_monitor_);
        if (maybe_current_game_value.has_value()) {
            return comparison_is_satisfied(maybe_current_game_value.value()) ? 1 : 0;
        }
    }
    return 0;
}

bool ImageStateMonitor::comparison_is_satisfied(Decimal current_game_value) const
{
    bool comparison_result = false;
    switch (dcs_id_compare_condition_) {
    case Comparison::EQUAL_TO:
        comparison_result = (current_game_value == dcs_id_comparison_value_);
        break;
    case Comparison::LESS_THAN:
        comparison_result = (current_game_value < dcs_id_comparison_value_);
        break;
    case Comparison::GREATER_THAN:
        comparison_result = (current_game_value > dcs_id_comparison_value_);
        break;
    }
    return comparison_result;
}
