// Copyright 2021 Charles Tytler

#include "pch.h"

#include "../../Utilities/StringUtilities.h"
#include "ComparisonMonitor.h"

ComparisonMonitor::ComparisonMonitor(const json &settings) { update_settings(settings); }

void ComparisonMonitor::update_settings(const json &settings)
{
    const std::string dcs_id_compare_monitor_raw = EPLJSONUtils::GetStringByName(settings, "dcs_id_compare_monitor");
    const std::string dcs_id_compare_condition_raw =
        EPLJSONUtils::GetStringByName(settings, "dcs_id_compare_condition");
    const std::string dcs_id_comparison_value_raw = EPLJSONUtils::GetStringByName(settings, "dcs_id_comparison_value");

    const bool compare_monitor_is_populated = is_integer(dcs_id_compare_monitor_raw);
    const bool comparison_value_is_populated = is_number(dcs_id_comparison_value_raw);
    settings_are_filled_ = compare_monitor_is_populated && comparison_value_is_populated;

    if (settings_are_filled_) {
        dcs_id_compare_monitor_ = std::stoi(dcs_id_compare_monitor_raw);
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

int ComparisonMonitor::determineContextState(DcsInterface &dcs_interface)
{
    if (settings_are_filled_) {
        const auto maybe_current_game_value = dcs_interface.get_decimal_of_dcs_id(dcs_id_compare_monitor_);
        if (maybe_current_game_value.has_value()) {
            return comparison_is_satisfied(maybe_current_game_value.value()) ? 1 : 0;
        }
    }
    return 0;
}

bool ComparisonMonitor::comparison_is_satisfied(Decimal current_game_value)
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
