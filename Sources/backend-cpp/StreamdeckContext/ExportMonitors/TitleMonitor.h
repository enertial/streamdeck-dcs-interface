// Copyright 2021 Charles Tytler

#pragma once

#include "ElgatoSD/EPLJSONUtils.h"
#include "SimulatorInterface/SimulatorInterface.h"

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
     * @param simulator_interface Interface to request current game state from.
     * @return The string that the Title should be set to if all settings are filled.
     */
    std::string determineTitle(SimulatorInterface *simulator_interface);

  private:
    /**
     * @brief Converts game string value to a title according to settings.
     *
     * @param current_game_value
     */
    std::string convertGameStateToTitle(const std::string &current_game_value);

    bool string_monitor_is_set_ = false; // True if all DCS ID string monitor settings have been set.

    SimulatorAddress monitor_address_; // Simulator address to monitor for context title.
    int vertical_spacing_ = 0;         // Vertical spacing (number of '\n') to include before or after title.
    bool modify_string_ = false;       // Flag set if string should be modified before using as title.
    std::unordered_map<std::string, std::string>
        string_monitor_mapping_; // Map of received values to title text to display on context.
};
