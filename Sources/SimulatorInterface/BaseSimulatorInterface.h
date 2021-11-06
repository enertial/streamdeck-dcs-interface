// Copyright 2020 Charles Tytler

#pragma once

#include "Utilities/Decimal.h"
#include "Utilities/UdpSocket.h"

#include <map>
#include <optional>
#include <string>
#include <vector>

using SimulatorConnectionSettings = struct {
    std::string rx_port;           // UDP port to receive updates from simulator.
    std::string tx_port;           // UDP port to send commands to simulator.
    std::string ip_address;        //  UDP IP address to send commands to simulator (Default is LocalHost).
    std::string multicast_address; //  UDP Multicast address group to join.
};

class BaseSimulatorInterface
{
  public:
    BaseSimulatorInterface(const SimulatorConnectionSettings &settings);

    /**
     * @brief Checks if the provided connection settings match the internally stored settings.
     *
     * @param settings Connection settings used for simulator socket.
     * @return True if provided connection settings match internal settings.
     */
    bool connection_settings_match(const SimulatorConnectionSettings &settings);

    /**
     * @brief Receives simulator state broadcasts, updating internal current game state.
     *
     */
    virtual void update_simulator_state(){};

    /**
     * @brief Sends a message to simulator to command a change of an object's value.
     *
     * @param button_id ID number of the button.
     * @param device_id ID number of the device.
     * @param value     Value to set the button to.
     */
    virtual void send_simulator_command(const int button_id, const std::string &device_id, const std::string &value){};

    /**
     * @brief Sends a reset command to simulator to signify a request for a resend of data.
     *
     */
    virtual void send_simulator_reset_command(){};

    /**
     * @brief Get the name of the current simulator module.
     *
     * @return Aircraft module name.
     */
    std::string get_current_simulator_module();

    /**
     * @brief Get the value of object from current game state.
     *
     * @return Optional value of object ID is returned if a value has been logged.
     */
    std::optional<std::string> get_value_of_simulator_object_state(const int object_id);

    /**
     * @brief Get the value as Decimal of object from current game state.
     *
     * @return Optional value of object is returned if a value has been logged that is a Decimal.
     */
    std::optional<Decimal> get_decimal_of_simulator_object_state(const int object_id);

    /**
     * @brief Clears history of logged current game state values.
     *
     */
    void clear_game_state();

    /**
     * @brief For debugging purposes, outputs all logged object key value pairs stored in current game state.
     *
     * @return Map of object IDs and their values in current game state.
     */
    std::map<int, std::string> debug_get_current_game_state();

  protected:
    UdpSocket simulator_socket_;      // UDP Socket connection for communicating with simulator.
    std::string current_game_module_; // Stores the current module name being used in game.
    std::map<int, std::string>
        current_game_state_; // Maps object ID keys of received values to their most recently published values.

  private:
    SimulatorConnectionSettings connection_settings_; // Stored connection settings used for simulator socket.
};
