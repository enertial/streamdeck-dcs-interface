// Copyright 2021 Charles Tytler

#pragma once

#include "../../Common/EPLJSONUtils.h"
#include "../../DcsInterface/DcsInterface.h"
#include "../../Utilities/Decimal.h"

#include <string>

class IncrementMonitor
{
  public:
    IncrementMonitor() = default;
    IncrementMonitor(const json &settings);

    /**
     * @brief Updates internal monitor conditions based on user settings.
     *
     * @param settings Json settings from Streamdeck property inspector.
     */
    void update_settings(const json &settings);

    /**
     * @brief Keeps the increment status up to date with game state.
     *
     * @param dcs_interface Interface to request current game state from.
     */
    void update(DcsInterface &dcs_interface);

    /**
     * @brief Applies a commanded delta to the internal increment and returns the new current value.
     *
     * @param delta_cmd The change from current increment being commanded by user.
     * @param increment_min The minimum value to clamp the increment to.
     * @param increment_max The maximum value to clamp the increment to.
     * @param cycling_is_allowed Flag to indicate that if the min/max is reached, the increment should wrap around to
     * other end of range.
     * @return Updated current value of the increment.
     */
    Decimal get_increment_after_command(const Decimal &delta_cmd,
                                        const Decimal &increment_min,
                                        const Decimal &increment_max,
                                        const bool cycling_is_allowed);

  private:
    // Status of user-filled fields.
    bool increment_monitor_is_set_ = false; // True if a DCS ID increment monitor setting has been set.

    Decimal current_increment_value_; // Stored value for increment button types.

    // Stored settings extracted from user-filled fields.
    int dcs_id_increment_monitor_ = 0; // DCS ID to monitor for updating current increment value from game state.
};
