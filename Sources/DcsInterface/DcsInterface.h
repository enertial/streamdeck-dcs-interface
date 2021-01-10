// Copyright 2020 Charles Tytler

#pragma once

#include "Utilities/DcsSocket.h"

#include <map>
#include <string>
#include <vector>

using DcsConnectionSettings = struct {
    std::string rx_port;    // UDP port to receive updates from DCS.
    std::string tx_port;    // UDP port to send commands to DCS.
    std::string ip_address; //  UDP IP address to send commands to DCS (Default is LocalHost).
};

class DcsInterface {

  public:
    /**
     * @brief Construct a new Dcs Interface object
     *
     * @param settings Connection settings to use for DCS Socket.
     */
    DcsInterface(const DcsConnectionSettings &settings);

    /**
     * @brief Checks if the provided connection settings match the internally stored settings.
     *
     * @param settings Connection settings used for DCS Socket.
     * @return True if provided connection settings match internal settings.
     */
    bool connection_settings_match(const DcsConnectionSettings &settings);

    /**
     * @brief Receives DCS value updates, updating DcsInterface's internal current game state.
     *
     */
    void update_dcs_state();

    /**
     * @brief Get the name of the current DCS aircraft module.
     *
     * @return Aircraft module name.
     */
    std::string get_current_dcs_module();

    /**
     * @brief Get the value of dcs id object from current game state.
     *
     * @return Value of DCS ID, defaults to "" if DCS ID has not been logged.
     */
    std::string get_value_of_dcs_id(const int dcs_id);

    /**
     * @brief Sends a message to DCS to command a change in a clickable data item.
     *
     * @param button_id ID number of the button.
     * @param device_id ID number of the device.
     * @param value     Value to set the button to.
     */
    void send_dcs_command(const int button_id, const std::string &device_id, const std::string &value);

    /**
     * @brief Sends a reset command ("R" char) to DCS to signify a request for a resend of data.
     *
     */
    void send_dcs_reset_command();

    /**
     * @brief Clears history of logged DCS current game state values.
     *
     */
    void clear_game_state();

    /**
     * @brief For debugging purposes, outputs all logged DCS ID key value pairs stored in current game state.
     *
     * @return Map of IDs and their values in current game state.
     */
    std::map<int, std::string> debug_get_current_game_state();

  private:
    /**
     * @brief Processes received tokens of DCS game updates.
     *
     * @param key Key for updated value
     * @param value Updated value.
     */
    void handle_received_token(const std::string &key, const std::string &value);

    DcsConnectionSettings connection_settings_; // Stored connection settings used for DCS Socket.
    DcsSocket dcs_socket_;                      // UDP Socket connection for communicating with DCS lua export scripts.
    std::string current_game_module_;           // Stores the current aircraft module name being used in game.
    std::map<int, std::string>
        current_game_state_; // Maps DCS ID keys of received values to their most recently published values.
};
