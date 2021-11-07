// Copyright 2021 Charles Tytler

#pragma once

#include "StreamdeckContext/StreamdeckContext.h"

#include <optional>
#include <string>

class MomentaryContext : public StreamdeckContext
{
  public:
    using StreamdeckContext::StreamdeckContext;

    /**
     * @brief Sends DCS commands according to button type and settings received during Key Down/Up event.
     *
     * @param dcs_interface Interface to DCS containing current game state.
     * @param mConnectionManager Interface to Streamdeck for current context.
     * @param payload Json payload received with KeyDown/KeyUp callback.
     */
    void handleButtonPressedEvent(SimulatorInterface &dcs_interface,
                                  ESDConnectionManager *mConnectionManager,
                                  const json &inPayload);

    void handleButtonReleasedEvent(SimulatorInterface &dcs_interface,
                                   ESDConnectionManager *mConnectionManager,
                                   const json &inPayload);
};
