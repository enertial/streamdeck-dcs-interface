// Copyright 2020 Charles Tytler

#pragma once

#include "Utilities/Decimal.h"
#include "Utilities/UdpSocket.h"
#include "Vendor/json/src/json.hpp"
using json = nlohmann::json;

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

enum class AddressType { ADDRESS_ONLY, INTEGER, STRING };

struct SimulatorAddress {
    AddressType type;
    unsigned int address;
    // Only for INTEGER:
    unsigned int mask;
    uint8_t shift;
    // Only for STRING:
    unsigned int max_length;

    SimulatorAddress(unsigned int address);
    SimulatorAddress(unsigned int address, unsigned int mask, uint8_t shift);
    SimulatorAddress(unsigned int address, unsigned int max_length);
};

class SimulatorInterface
{
  public:
    SimulatorInterface(const SimulatorConnectionSettings &settings);

    /**
     * @brief Checks if the provided connection settings match the internally stored settings.
     * @param settings Connection settings used for simulator socket.
     * @return True if provided connection settings match internal settings.
     */
    bool connection_settings_match(const SimulatorConnectionSettings &settings) const;

    /**
     * @brief Get the name of the current simulator module.
     * @return Aircraft module name.
     */
    std::string get_current_simulator_module() const;

    /**
     * @brief Receives simulator state broadcasts, updating internal current game state.
     */
    virtual void update_simulator_state() = 0;

    /**
     * @brief Sends a message to simulator to command a change of an object's value.
     * @param address Object name to set value of.
     * @param value   Value to set the button to.
     */
    virtual void send_simulator_command(const std::string &address, const std::string &value) = 0;

    /**
     * @brief Sends a reset command to simulator to signify a request for a resend of data.
     */
    virtual void send_simulator_reset_command() = 0;

    /**
     * @brief Get the value of object from current game state.
     * @return Optional value of object ID is returned if a value has been logged.
     */
    virtual std::optional<std::string> get_value_of_simulator_object_state(const SimulatorAddress &address) const = 0;

    std::optional<std::string> get_value_of_simulator_object_state(const int address) const // Convenience overload.
    {
        return get_value_of_simulator_object_state(SimulatorAddress(address));
    }

    /**
     * @brief Get the value as Decimal of object from current game state.
     * @return Optional value of object is returned if a value has been logged that is a Decimal.
     */
    virtual std::optional<Decimal> get_decimal_of_simulator_object_state(const SimulatorAddress &address) const = 0;

    std::optional<Decimal> get_decimal_of_simulator_object_state(const int address) const // Convenience overload.
    {
        return get_decimal_of_simulator_object_state(SimulatorAddress(address));
    }

    /**
     * @brief For debugging purposes, outputs all logged object key value pairs stored in current game state.
     * @return Json representation of object IDs and their values in current game state.
     */
    virtual json debug_get_current_game_state() const = 0;

  protected:
    UdpSocket simulator_socket_;      // UDP Socket connection for communicating with simulator.
    std::string current_game_module_; // Stores the current module name being used in game.

  private:
    SimulatorConnectionSettings connection_settings_; // Stored connection settings used for simulator socket.
};
