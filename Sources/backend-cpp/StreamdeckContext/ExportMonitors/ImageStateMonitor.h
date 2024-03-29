// Copyright 2021 Charles Tytler

#pragma once

#include "ElgatoSD/EPLJSONUtils.h"
#include "SimulatorInterface/SimulatorInterface.h"
#include "Utilities/Decimal.h"

#include <optional>
class ImageStateMonitor
{
  public:
    ImageStateMonitor() = default;
    ImageStateMonitor(const json &settings);

    /**
     * @brief Updates internal monitor conditions based on user settings.
     *
     * @param settings Json settings from Streamdeck property inspector.
     */
    void update_settings(const json &settings);

    /**
     * @brief Determines what the context state should be according to current the current game state.
     *
     * @param simulator_interface Interface to request current game state from.
     * @return The Streamdeck context should be set to if all settings are filled.
     */
    int determineContextState(SimulatorInterface *simulator_interface) const;

  private:
    enum class Comparison { GREATER_THAN, EQUAL_TO, LESS_THAN };

    /**
     * @brief Checks if current game value satisfies the comparison criteria configured in settings.
     *
     * @param current_game_value
     */
    bool comparison_is_satisfied(Decimal current_game_value) const;

    // Status of user-filled fields.
    bool settings_are_filled_ = false; // True if all DCS ID comparison monitor settings have been set.

    // Stored settings extracted from user-filled fields.
    SimulatorAddress dcs_id_compare_monitor_{0};                     // Simulator address to monitor for state change.
    Comparison dcs_id_compare_condition_ = Comparison::GREATER_THAN; // Comparison to use for DCS ID compare monitor.
    Decimal dcs_id_comparison_value_; // Value to compare DCS ID compare monitor value to.
};
