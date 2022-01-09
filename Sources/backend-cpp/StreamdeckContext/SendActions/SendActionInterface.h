// Copyright 2022 Charles Tytler

#pragma once

#include "ElgatoSD/ESDConnectionManager.h"
#include "SimulatorInterface/SimulatorInterface.h"

class SendActionInterface
{

  public:
    SendActionInterface() = default;

    /**
     * @brief Sends simulator commands according to button type and settings received during Key Pressed and Released
     * events.
     *
     * @param simulator_interface Interface to simulator containing current game state.
     * @param mConnectionManager Interface to StreamDeck.
     * @param payload Json payload received with KeyDown/KeyUp callback.
     */
    virtual void handleButtonPressedEvent(const std::unique_ptr<SimulatorInterface> &simulator_interface,
                                          ESDConnectionManager *mConnectionManager,
                                          const json &inPayload) = 0;

    virtual void handleButtonReleasedEvent(const std::unique_ptr<SimulatorInterface> &simulator_interface,
                                           ESDConnectionManager *mConnectionManager,
                                           const json &inPayload) = 0;

    // For some actions (i.e. switches) a delay before forcing a state update is desired to avoid jittering and a race
    // condition of Streamdeck and Plugin trying to change state.
    bool delay_send_state() { return delay_send_state_; }

  protected:
    bool delay_send_state_ = false;
};
