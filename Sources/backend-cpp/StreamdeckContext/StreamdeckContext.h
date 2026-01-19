// Copyright 2020 Charles Tytler

#pragma once

#include "ElgatoSD/ESDConnectionManager.h"
#include "SimulatorInterface/SimulatorInterface.h"
#include "SimulatorInterface/SimulatorProtocolTypes.h"
#include "StreamdeckContext/ExportMonitors/ImageStateMonitor.h"
#include "StreamdeckContext/ExportMonitors/IncrementMonitor.h"
#include "StreamdeckContext/ExportMonitors/TitleMonitor.h"
#include "StreamdeckContext/SendActions/SendActionInterface.h"
#include "Utilities/StringUtilities.h"

#include <memory>
#include <optional>
#include <string>

class StreamdeckContext
{
  public:
    StreamdeckContext() = default;
    StreamdeckContext(const std::string &action, const std::string &context, const json &settings);

    // Detect if constructed context is valid.
    bool is_valid();

    // Simulation interface protocol to use for this context.
    Protocol protocol();

    /**
     * @brief Queries the simulator_interface for updates to the Context's monitored DCS IDs.
     *
     * @param simulator_interface Interface to simulator containing current game state.
     * @param mConnectionManager Interface to StreamDeck.
     */
    void updateContextState(SimulatorInterface *simulator_interface, ESDConnectionManager *mConnectionManager);

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
     * @brief Sends simulator commands according to button type and settings received during Key Pressed and Released
     * events.
     *
     * @param simulator_interface Interface to simulator containing current game state.
     * @param mConnectionManager Interface to StreamDeck.
     * @param payload Json payload received with KeyDown/KeyUp callback.
     */
    void handleButtonPressedEvent(SimulatorInterface *simulator_interface,
                                  ESDConnectionManager *mConnectionManager,
                                  const json &inPayload);

    void handleButtonReleasedEvent(SimulatorInterface *simulator_interface,
                                   ESDConnectionManager *mConnectionManager,
                                   const json &inPayload);

    /**
     * @brief Handles encoder rotation events.
     *
     * @param simulator_interface Interface to simulator containing current game state.
     * @param mConnectionManager Interface to StreamDeck.
     * @param payload Json payload received with encoder rotation callback.
     * @param ticks Number of rotation ticks (positive = clockwise, negative = counter-clockwise).
     */
    void handleEncoderRotation(SimulatorInterface *simulator_interface,
                              ESDConnectionManager *mConnectionManager,
                              const json &inPayload,
                              int ticks);

    /**
     * @brief Handles encoder press events.
     *
     * @param simulator_interface Interface to simulator containing current game state.
     * @param mConnectionManager Interface to StreamDeck.
     * @param payload Json payload received with encoder press callback.
     */
    void handleEncoderPress(SimulatorInterface *simulator_interface,
                           ESDConnectionManager *mConnectionManager,
                           const json &inPayload);

    static const int NUM_FRAMES_DELAY_FORCED_STATE_UPDATE = 3; // Kept public for unit testing.

  private:
    std::string context_; // Unique context ID used by Streamdeck to refer to instances of buttons.
    Protocol protocol_;   // Simulation interface protocol this instance will communicate with.

    // Mutable context state.
    int current_state_ = 0;          // Stored state of the context.
    std::string current_title_ = ""; // Stored title of the context.
    std::string last_encoder_display_value_ = ""; // Last value displayed on encoder LCD.
    std::string last_encoder_image_path_ = ""; // Last image path set for encoder background.
    json settings_;                  // Stored settings for this context.

    // Monitors.
    ImageStateMonitor comparison_monitor_{}; // Monitors DCS ID to determine the image state of Streamdeck context.
    TitleMonitor title_monitor_{};           // Monitors DCS ID to determine the title text of Streamdeck context.
    // SendAction.
    std::unique_ptr<SendActionInterface> send_action_{}; // Holds the actions to take on Button Pressed/Released events.

    // Optional settings.
    std::optional<int> delay_for_force_send_state_; // When populated, requests a force send of state to Streamdeck
                                                    // after counting down the stored delay value.
};
