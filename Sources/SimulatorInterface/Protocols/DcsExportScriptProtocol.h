// Copyright 2020 Charles Tytler

#pragma once

#include "SimulatorInterface/SimulatorInterface.h"

class DcsExportScriptProtocol : public SimulatorInterface
{
  public:
    DcsExportScriptProtocol(const SimulatorConnectionSettings &settings);

    /**
     * @brief Receives simulator state broadcasts, updating internal current game state.
     *
     */
    void update_simulator_state();

    /**
     * @brief Sends a message to simulator to command a change of an object's value.
     *
     * @param button_id ID number of the button.
     * @param device_id ID number of the device.
     * @param value     Value to set the button to.
     */
    void send_simulator_command(const int button_id, const std::string &device_id, const std::string &value);

    /**
     * @brief Sends a reset command to simulator to signify a request for a resend of data.
     *
     */
    void send_simulator_reset_command();

  private:
    /**
     * @brief Processes received tokens of simulator game updates.
     *
     * @param key Key for updated value
     * @param value Updated value.
     */
    void handle_received_token(const std::string &key, const std::string &value);
};
