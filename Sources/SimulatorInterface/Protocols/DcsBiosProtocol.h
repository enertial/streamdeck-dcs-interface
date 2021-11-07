// Copyright 2020 Charles Tytler

#pragma once

#include "SimulatorInterface/SimulatorInterface.h"

class DcsBiosProtocol : public SimulatorInterface
{
  public:
    DcsBiosProtocol(const SimulatorConnectionSettings &settings);

    /**
     * @brief Receives simulator state broadcasts, updating internal current game state.
     *
     */
    void update_simulator_state();

    /**
     * @brief Sends a message to simulator to command a change of an object's value.
     *
     * @param address Object name to set value of.
     * @param value   Value to set the button to.
     */
    void send_simulator_command(const std::string &address, const std::string &value);

    /**
     * @brief Sends a reset command to simulator to signify a request for a resend of data.
     *
     */
    void send_simulator_reset_command();

  private:
    /**
     * @brief Processes received data of simulator updates.
     *
     * @param message Buffer of data receieved from DCS BIOS UDP socket.
     */
    void handle_received_message(const unsigned char *message);
};
