// Copyright 2021 Charles Tytler

#pragma once

#include "StreamdeckContext/StreamdeckContext.h"

#include <optional>
#include <string>

class IncrementContext : public StreamdeckContext
{
  public:
    using StreamdeckContext::StreamdeckContext;

    /**
     * @brief Sends DCS commands according to button type and settings received during Key Pressed and Released events.
     *
     * @param dcs_interface Interface to DCS containing current game state.
     * @param mConnectionManager Interface to StreamDeck.
     * @param payload Json payload received with KeyDown/KeyUp callback.
     */
    void handleButtonPressedEvent(SimulatorInterface &dcs_interface,
                                  ESDConnectionManager *mConnectionManager,
                                  const json &inPayload);

    void handleButtonReleasedEvent(SimulatorInterface &dcs_interface,
                                   ESDConnectionManager *mConnectionManager,
                                   const json &inPayload);

  private:
    /**
     * @brief Determines the send value for the type of button for KeyDown and KeyUp events.
     *
     * @param settings Settings for context.
     * @return (Optional) Value to be sent to Button ID if it exists.
     */
    std::optional<std::string> determineSendValue(const json &settings);
};
