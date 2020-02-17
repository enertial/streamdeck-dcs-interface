// Copyright 2020 Charles Tytler

#pragma once

#include "DcsSocket.h"

#include "Common\ESDConnectionManager.h"

#include <string>
#include <queue>
#include <unordered_map>

class DcsInterface
{
    // Maps registered DCS IDs to a vector of Stream Deck button instances.
    using dcs_id_to_context_map = std::unordered_map<int, std::vector<std::string>>;

public:
    DcsInterface(const int rx_port, const int tx_port);

    /**
     * @brief Registers a new instance of a StreamDeck button that wishes to monitor the value of a given DCS ID.
     *
     * @param dcs_id  DCS ID to monitor.
     * @param context Context identification of StreamDeck button registered to DCS ID updates.
     */
    void register_dcs_monitor(const int dcs_id, const std::string &context);

    /**
     * @brief Monitors DCS events and processes them, updating registered StreamDeck button instances.
     *
     */
    // TODO: add ESDConnectionManager &mConnectionManager as input to update.
    void process_next_dcs_events();

    /**
     * @brief Handles key press using desired action and attributes specified from StreamDeck property instpector.
     * 
     * @param action     StreamDeck button instance type implying desired type of action.
     * @param in_payload User-defined attributes extracted from StreamDeck button property inspector.
     */
    void handle_key_down(const std::string action, const char *in_payload);

    /**
     * @brief Handles key release using desired action and attributes specified from StreamDeck property instpector.
     * 
     * @param action     StreamDeck button instance type implying desired type of action.
     * @param in_payload User-defined attributes extracted from StreamDeck button property inspector.
     */
    void handle_key_up(const std::string action, const char *in_payload);

private:
    /**
     * @brief Handles StreamDeck button updates to apply (if any) based on received DCS event.
     * 
     * @param dcs_id DCS ID with a received change in value.
     * @param value  Updated value of the DCS ID.
     */
    void handle_dcs_event(const int dcs_id, const std::string value);

    DcsSocket dcs_socket_;
    dcs_id_to_context_map dcs_monitor_map_ = {};
};
