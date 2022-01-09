// Copyright 2021 Charles Tytler

#pragma once

#include "StreamdeckContext/SendActions/SendActionInterface.h"

#include <optional>
#include <string>

class SwitchAction : public SendActionInterface
{
  public:
    SwitchAction() { delay_send_state_ = true; }

    /**
     * @brief Sends simulator commands according to button type and settings received during Key Pressed and Released
     * events.
     *
     * @param simulator_interface Interface to simulator containing current game state.
     * @param mConnectionManager Interface to StreamDeck.
     * @param payload Json payload received with KeyDown/KeyUp callback.
     */
    void handleButtonPressedEvent(const std::unique_ptr<SimulatorInterface> &simulator_interface,
                                  ESDConnectionManager *mConnectionManager,
                                  const json &inPayload);

    void handleButtonReleasedEvent(const std::unique_ptr<SimulatorInterface> &simulator_interface,
                                   ESDConnectionManager *mConnectionManager,
                                   const json &inPayload);
};
