// Copyright 2020 Charles Tytler

#pragma once

#include "DcsInterface/DcsInterface.h"
#include "StreamdeckContext/ExportMonitors/ComparisonMonitor.h"
#include "StreamdeckContext/ExportMonitors/IncrementMonitor.h"
#include "StreamdeckContext/ExportMonitors/TitleMonitor.h"
#include "Utilities/StringUtilities.h"

#ifndef UNIT_TEST
#include "ElgatoSD/ESDConnectionManager.h"
#endif

#include <optional>
#include <string>

enum class KeyEvent { PRESSED, RELEASED };
enum class ContextState { FIRST, SECOND };

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
     * @param payload Json payload received with KeyDown/KeyUp callback.
     */
    virtual void handleButtonEvent(DcsInterface &dcs_interface, const KeyEvent event, const json &inPayload) = 0;

    // Monitors.
    ComparisonMonitor comparison_monitor_{}; // Monitors DCS ID to determine the image state of Streamdeck context.
    TitleMonitor title_monitor_{};           // Monitors DCS ID to determine the title text of Streamdeck context.
    IncrementMonitor increment_monitor_{};   // Monitors DCS ID to determine the state of an incremental switch.

  private:
    std::string context_; // Unique context ID used by Streamdeck to refer to instances of buttons.

    // Optional settings.
    std::optional<int> delay_for_force_send_state_; // When populated, requests a force send of state to Streamdeck
                                                    // after counting down the stored delay value.

    // Context state.
    int current_state_ = 0;              // Stored state of the context.
    std::string current_title_ = "";     // Stored title of the context.
    bool disable_release_value_ = false; // Flag set by user settings for momentary button types.
};
