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
     * @param event Type of button event - PRESSED or RELEASED.
     * @param action Type of button action - used to determine momentary, swtich, or increment button type.
     * @param payload Json payload received with KeyDown/KeyUp callback.
     */
    void handleButtonEvent(DcsInterface &dcs_interface, const KeyEvent event, const json &inPayload);

  private:
    /**
     * @brief Determines the send value for the type of button for KeyDown and KeyUp events.
     *
     * @param event Either a KeyDown or KeyUp event.
     * @param settings Settings for context.
     * @return (Optional) Value to be sent to Button ID if it exists.
     */
    std::optional<std::string> determineSendValue(const KeyEvent event, const json &settings) const;
};
