// Copyright 2020 Charles Tytler

#pragma once

#include "Utilities/Decimal.h"
#include "Utilities/UdpSocket.h"

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

using SimulatorConnectionSettings = struct {
    std::string rx_port;           // UDP port to receive updates from simulator.
    std::string tx_port;           // UDP port to send commands to simulator.
    std::string ip_address;        //  UDP IP address to send commands to simulator (Default is LocalHost).
    std::string multicast_address; //  UDP Multicast address group to join.
};

class SimulatorInterface
{
  public:
    SimulatorInterface(const SimulatorConnectionSettings &settings);

    /**
     * @brief Checks if the provided connection settings match the internally stored settings.
     *
     * @param settings Connection settings used for simulator socket.
     * @return True if provided connection settings match internal settings.
     */
    bool connection_settings_match(const SimulatorConnectionSettings &settings) const;

    /**
     * @brief Receives simulator state broadcasts, updating internal current game state.
     *
     */
    virtual void update_simulator_state(){};

    /**
     * @brief Sends a message to simulator to command a change of an object's value.
     *
     * @param address Object name to set value of.
     * @param value   Value to set the button to.
     */
    virtual void send_simulator_command(const std::string &address, const std::string &value){};

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
    std::string get_current_simulator_module() const;

    /**
     * @brief Get the value of object from current game state.
     *
     * @return Optional value of object ID is returned if a value has been logged.
     */
    std::optional<std::string> get_value_of_simulator_object_state(const int object_id) const;

    /**
     * @brief Get the value as Decimal of object from current game state.
     *
     * @return Optional value of object is returned if a value has been logged that is a Decimal.
     */
    std::optional<Decimal> get_decimal_of_simulator_object_state(const int object_id) const;

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
    std::unordered_map<int, std::string> debug_get_current_game_state() const;

  protected:
    UdpSocket simulator_socket_;      // UDP Socket connection for communicating with simulator.
    std::string current_game_module_; // Stores the current module name being used in game.
    std::unordered_map<int, std::string>
        current_game_state_; // Maps object ID keys of received values to their most recently published values.

  private:
    SimulatorConnectionSettings connection_settings_; // Stored connection settings used for simulator socket.
};
