// Copyright 2020 Charles Tytler

#include "pch.h"

#include "DcsInterface.h"

DcsInterface::DcsInterface(const std::string &rx_port, const std::string &tx_port, const std::string &ip_address)
    : dcs_socket_(rx_port, tx_port, ip_address)
{
}

std::vector<DcsIdValueUpdate> DcsInterface::get_next_dcs_update()
{
    // Receive next UDP message from DCS and strip header.
    const char header_delimiter = '*'; // Header content ends in a '*'.
    std::stringstream recv_msg = dcs_socket_.DcsReceive();

    // Iterate through tokens received from single message.
    std::vector<DcsIdValueUpdate> received_updates = {};
    std::string token;
    if (std::getline(recv_msg, token, header_delimiter))
    {
        const char token_delimiter = ':';
        while (std::getline(recv_msg, token, token_delimiter))
        {
            // Parse token string of the form:
            //   "<dcs_id>=<reported_value>"
            const std::string delim = "=";
            const auto delim_loc = token.find(delim);
            const int dcs_id = std::stoi(token.substr(0, delim_loc));
            const std::string reported_value = token.substr(delim_loc + delim.size(), token.size());

            DcsIdValueUpdate value_update;
            value_update.dcs_id = dcs_id;
            value_update.dcs_id_value = reported_value;
            received_updates.push_back(std::move(value_update));
        }
    }
    return received_updates;
}

void DcsInterface::send_dcs_command(const int button_id, const std::string &device_id, const std::string &value)
{
    const std::string message_assembly = "C" + device_id + "," + std::to_string(3000 + button_id) + "," + value;
    dcs_socket_.DcsSend(message_assembly);
}

void DcsInterface::clear_game_state()
{
    current_game_state_.clear();
}
