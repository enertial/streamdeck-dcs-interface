// Copyright 2021 Charles Tytler

#include "TitleMonitor.h"

#include "Utilities/StringUtilities.h"

TitleMonitor::TitleMonitor(const json &settings) { update_settings(settings); }

void TitleMonitor::update_settings(const json &title_monitor_settings)
{
    string_monitor_is_set_ = title_monitor_settings["is_set"];
    if (string_monitor_is_set_) {
        SimulatorAddress address;
        if (title_monitor_settings["monitor_address"]["type"] == "STRING") {
            address = SimulatorAddress{title_monitor_settings["monitor_address"][0]["address"],
                                       title_monitor_settings["monitor_address"][0]["max_len"]};
        } else if (title_monitor_settings["monitor_address"]["type"] == "INTEGER") {
            address = SimulatorAddress{title_monitor_settings["monitor_address"]["address"],
                                       title_monitor_settings["monitor_address"]["mask"],
                                       title_monitor_settings["monitor_address"]["shift"]};
        } else if (title_monitor_settings["monitor_address"]["type"] == "ADDRESS_ONLY") {
            address = SimulatorAddress{title_monitor_settings["monitor_address"]["address"]};
        }

        modify_string_ = false; // TODO: Handle modification of strings
        vertical_spacing_ =
            title_monitor_settings.contains("vertical_spacing") ? title_monitor_settings["vertical_spacing"] : 0;
    }
}

std::string TitleMonitor::determineTitle(SimulatorInterface *simulator_interface)
{
    std::string updated_title = "";

    if (string_monitor_is_set_) {
        const std::optional<std::string> maybe_current_game_value =
            simulator_interface->get_string_at_addr(monitor_address_);
        if (maybe_current_game_value.has_value()) {
            updated_title = convertGameStateToTitle(maybe_current_game_value.value());
        }
    }

    return updated_title;
}

std::string TitleMonitor::convertGameStateToTitle(const std::string &current_game_value)
{
    std::string title;
    if (modify_string_) {
        title = string_monitor_mapping_[current_game_value];
    } else {
        title = current_game_value;
    }
    // Apply vertical spacing.
    if (vertical_spacing_ < 0) {
        for (int i = 0; i > vertical_spacing_; --i) {
            title = "\n" + title;
        }
    } else {
        for (int i = 0; i < vertical_spacing_; ++i) {
            title = title + "\n";
        }
    }
    return title;
}