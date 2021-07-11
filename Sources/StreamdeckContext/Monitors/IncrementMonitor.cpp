// Copyright 2021 Charles Tytler

#include "pch.h"

#include "../../Utilities/StringUtilities.h"
#include "IncrementMonitor.h"

IncrementMonitor::IncrementMonitor(const json &settings) { update_settings(settings); }

void IncrementMonitor::update_settings(const json &settings)
{
    const std::string dcs_id_increment_monitor_raw =
        EPLJSONUtils::GetStringByName(settings, "dcs_id_increment_monitor");

    // Process status of settings.
    increment_monitor_is_set_ = is_integer(dcs_id_increment_monitor_raw);

    // Update internal settings of class instance.
    if (increment_monitor_is_set_) {
        dcs_id_increment_monitor_ = std::stoi(dcs_id_increment_monitor_raw);
    }
}

void IncrementMonitor::update(DcsInterface &dcs_interface)
{
    if (increment_monitor_is_set_) {
        const std::optional<Decimal> maybe_current_game_value =
            dcs_interface.get_decimal_of_dcs_id(dcs_id_increment_monitor_);
        if (maybe_current_game_value.has_value()) {
            current_increment_value_ = maybe_current_game_value.value();
        }
    }
}

Decimal IncrementMonitor::get_increment_after_command(const Decimal &delta_cmd,
                                                      const Decimal &increment_min,
                                                      const Decimal &increment_max,
                                                      const bool cycling_is_allowed)
{
    current_increment_value_ += delta_cmd;

    if (current_increment_value_ < increment_min) {
        current_increment_value_ = cycling_is_allowed ? increment_max : increment_min;
    } else if (current_increment_value_ > increment_max) {
        current_increment_value_ = cycling_is_allowed ? increment_min : increment_max;
    }
    return current_increment_value_;
}
