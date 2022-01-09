// Copyright 2021 Charles Tytler

#pragma once

#include "StreamdeckContext/SendActions/SendActionInterface.h"

#include <optional>
#include <string>

class MomentaryAction : public SendActionInterface
{
  public:
    MomentaryAction() = default;

    /**
     * @brief Sends simulator commands according to button type and settings received during Key Down/Up event.
     *
     * @param simulator_interface Interface to simulator containing current game state.
     * @param mConnectionManager Interface to Streamdeck for current context.
     * @param payload Json payload received with KeyDown/KeyUp callback.
     */
    void handleButtonPressedEvent(const std::unique_ptr<SimulatorInterface> &simulator_interface,
                                  ESDConnectionManager *mConnectionManager,
                                  const json &inPayload);

    void handleButtonReleasedEvent(const std::unique_ptr<SimulatorInterface> &simulator_interface,
                                   ESDConnectionManager *mConnectionManager,
                                   const json &inPayload);
};
