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

    using StateMonitorSettings = struct {
        SimulatorAddress address;
        Comparison comparison;
        Decimal comparison_value;
    };

    /**
     * @brief Checks if current game value satisfies the comparison criteria for state.
     */
    bool comparison_is_satisfied(const Decimal &current_game_value,
                                 const Comparison &comparison,
                                 const Decimal &comparison_value) const;

    // Status of user-filled fields.
    bool monitor_is_set_ = false; // True if all monitor settings have been set.

    // Stored settings extracted from user-filled fields.
    std::vector<StateMonitorSettings> settings_per_state_;
    Comparison dcs_id_compare_condition_ = Comparison::GREATER_THAN; // Comparison to use for DCS ID compare monitor.
    Decimal dcs_id_comparison_value_; // Value to compare DCS ID compare monitor value to.
};
