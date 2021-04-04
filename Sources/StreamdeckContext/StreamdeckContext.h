// Copyright 2020 Charles Tytler

#pragma once

#include "../DcsInterface/DcsInterface.h"
#include "../Utilities/Decimal.h"
#include "../Utilities/StringUtilities.h"
#include "Monitors/ComparisonMonitor.h"

#ifndef UNIT_TEST
#include "../Common/ESDConnectionManager.h"
#endif

#include <optional>
#include <string>

enum class KeyEvent { PRESSED, RELEASED };

class StreamdeckContext
{
  public:
    StreamdeckContext() = default;
    StreamdeckContext(const std::string &context);
    StreamdeckContext(const std::string &context, const json &settings);

    /**
     * @brief Queries the dcs_interface for updates to the Context's monitored DCS IDs.
     *
     * @param dcs_interface Interface to DCS containing current game state.
     * @param mConnectionManager Interface to StreamDeck.
     */
    void updateContextState(DcsInterface &dcs_interface, ESDConnectionManager *mConnectionManager);

    /**
     * @brief Forces an update to the Streamdeck of the context's current state be sent with current static values.
     *        (Normally an update is sent to the Streamdeck only on change of current state).
     *
     * @param mConnectionManager Interface to StreamDeck.
     */
    void forceSendState(ESDConnectionManager *mConnectionManager);

    /**
     * @brief Forces an update to the Streamdeck of the context's current state be sent after a specified delay.
     *        (Normally an update is sent to the Streamdeck only on change of current state).
     *
     * @param delay_count Number of frames before a force send of the context state is sent.
     */
    void forceSendStateAfterDelay(const int delay_count);

    /**
     * @brief Updates settings from received json payload.
     *
     * @param settings Json payload of settings values populated in Streamdeck Property Inspector.
     */
    void updateContextSettings(const json &settings);

    /**
     * @brief Sends DCS commands according to button type and settings received during Key Down/Up event.
     *
     * @param dcs_interface Interface to DCS containing current game state.
     * @param event Type of button event - KeyDown or KeyUp
     * @param action Type of button action - used to determine momentary, swtich, or increment button type.
     * @param payload Json payload received with KeyDown/KeyUp callback.
     */
    void handleButtonEvent(DcsInterface &dcs_interface,
                           const KeyEvent event,
                           const std::string &action,
                           const json &inPayload);

  private:
    /**
     * @brief Determines what the context title should be according to current game value and string monitor settings.
     *
     * @param current_game_string_value Value receieved from DCS.
     * @return std::string String that the Streamdeck context title should be set to.
     */
    std::string determineTitleForStringMonitor(const std::string &current_game_string_value);

    /**
     * @brief Determines the send value for the type of button for KeyDown and KeyUp events.
     *
     * @param event Either a KeyDown or KeyUp event.
     * @param state Current state of the context.
     * @param settings Settings for context.
     * @param value Value to be sent to Button ID.
     * @return True if value should be sent to DCS.
     */
    bool determineSendValueForMomentary(const KeyEvent event, const json &settings, std::string &value);
    bool determineSendValueForSwitch(const KeyEvent event, const int state, const json &settings, std::string &value);
    bool determineSendValueForIncrement(const KeyEvent event, const json &settings, std::string &value);

    std::string context_; // Unique context ID used by Streamdeck to refer to instances of buttons.

    // Monitors.
    ComparisonMonitor comparison_monitor_{}; // Monitors DCS ID to determine the image state of Streamdeck context.

    // Status of user-filled fields.
    bool increment_monitor_is_set_ = false; // True if a DCS ID increment monitor setting has been set.
    bool string_monitor_is_set_ = false;    // True if all DCS ID string monitor settings have been set.

    // Optional settings.
    std::optional<int> delay_for_force_send_state_; // When populated, requests a force send of state to Streamdeck
                                                    // after counting down the stored delay value.

    // Context state.
    int current_state_ = 0;                    // Stored state of the context.
    std::string current_title_ = "";           // Stored title of the context.
    Decimal current_increment_value_;          // Stored value for increment button types.
    bool cycle_increments_is_allowed_ = false; // Flag set by user settings for increment button types.
    bool disable_release_value_ = false;       // Flag set by user settings for momentary button types.

    // Stored settings extracted from user-filled fields.
    int dcs_id_increment_monitor_ = 0;        // DCS ID to monitor for updating current increment value from game state.
    int dcs_id_string_monitor_ = 0;           // DCS ID to monitor for context title.
    int string_monitor_vertical_spacing_ = 0; // Vertical spacing (number of '\n') to include before or after title.
    bool string_monitor_passthrough_ = true;  // Flag set by user to passthrough string to title unaltered.
    std::map<std::string, std::string>
        string_monitor_mapping_; // Map of received values to title text to display on context.
};
