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
    std::string context;      // Identification of a context registered to the DCS ID.
    int dcs_id;               // DCS ID which has received an updated value.
    std::string dcs_id_value; // Updated value.
} DcsIdValueUpdate;

class DcsInterface
{
    // Maps registered DCS IDs to a vector of Stream Deck button instances.
    using dcs_id_to_context_map = std::unordered_map<int, std::unordered_set<std::string>>;
    using context_to_dcs_id_map = std::unordered_map<std::string, int>;

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
     * @brief Registers a new instance of a StreamDeck button that wishes to monitor the value of a given DCS ID.
     *
     * @param dcs_id  DCS ID to monitor.
     * @param context Context identification of StreamDeck button registered to DCS ID updates.
     */
    void register_dcs_monitor(const int dcs_id, const std::string &context);

    /**
     * @brief Removes registration of a StreamDeck button monitoring the value of a DCS ID.
     *
     * @param context Context identification of StreamDeck button registered to DCS ID updates.
     */
    void unregister_dcs_monitor(const std::string &context);

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

private:
    DcsSocket dcs_socket_;                               // UDP Socket connection for communicating with DCS lua export scripts.
    dcs_id_to_context_map registered_contexts_map_ = {}; // Maps DCS ID keys to registered Streamdeck contexts.
    context_to_dcs_id_map active_contexts_map_ = {};     // Maps Streamdeck contexts to actively monitored DCS ID keys.
};
