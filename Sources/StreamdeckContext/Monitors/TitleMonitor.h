// Copyright 2021 Charles Tytler

#pragma once

#include "../../Common/EPLJSONUtils.h"
#include "../../DcsInterface/DcsInterface.h"

#include <string>

class TitleMonitor
{
  public:
    TitleMonitor() = default;
    TitleMonitor(const json &settings);

    /**
     * @brief Updates internal monitor conditions based on user settings.
     *
     * @param settings Json settings from Streamdeck property inspector.
     */
    void update_settings(const json &settings);

    /**
     * @brief Determines what the context title text should be according to current the current game state.
     *
     * @param dcs_interface Interface to request current game state from.
     * @return The string that the Title should be set to if all settings are filled.
     */
    std::string determineTitle(DcsInterface &dcs_interface);

  private:
    /**
     * @brief Converts game string value to a title according to settings.
     *
     * @param current_game_value
     */
    std::string convertGameStateToTitle(const std::string &current_game_value);

    bool string_monitor_is_set_ = false; // True if all DCS ID string monitor settings have been set.

    int dcs_id_string_monitor_ = 0;           // DCS ID to monitor for context title.
    int string_monitor_vertical_spacing_ = 0; // Vertical spacing (number of '\n') to include before or after title.
    bool string_monitor_passthrough_ = true;  // Flag set by user to passthrough string to title unaltered.
    std::map<std::string, std::string>
        string_monitor_mapping_; // Map of received values to title text to display on context.
};
