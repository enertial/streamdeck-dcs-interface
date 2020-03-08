// Copyright 2020 Charles Tytler

#pragma once

#include "DcsSocket.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

/**
 * @brief Defines a per-context update of a DCS ID value.
 *
 */
typedef struct
{
    int dcs_id;               // DCS ID which has received an updated value.
    std::string dcs_id_value; // Updated value.
} DcsIdValueUpdate;

class DcsInterface
{

public:
    /**
     * @brief Construct a new Dcs Interface object
     *
     * @param rx_port UDP port to receive updates from DCS.
     * @param tx_port UDP port to send commands to DCS.
     * @param ip_address UDP IP address to send commands to DCS (Default is LocalHost).
     */
    DcsInterface(const std::string &rx_port, const std::string &tx_port, const std::string &ip_address);

    /**
     * @brief Receives DCS value updates, returning ID values which are monitored by a Streamdeck context.
     *
     * @return A vector of value updates for monitored DCS ID's and their registered context.
     */
    std::vector<DcsIdValueUpdate> get_next_dcs_update();

    /**
     * @brief Sends a message to DCS to command a change in a clickable data item.
     *
     * @param button_id ID number of the button.
     * @param device_id ID number of the device.
     * @param value     Value to set the button to.
     */
    void send_dcs_command(const int button_id, const std::string &device_id, const std::string &value);

    /**
     * @brief Clears history of logged DCS current game state values.
     * 
     */
    void clear_game_state();

private:
    DcsSocket dcs_socket_;                                    // UDP Socket connection for communicating with DCS lua export scripts.
    std::unordered_map<int, std::string> current_game_state_; // Maps DCS ID keys of received values to their most recently published values.
};
