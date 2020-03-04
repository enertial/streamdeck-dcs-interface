// Copyright 2020 Charles Tytler

#include "pch.h"

#include "DcsInterface.h"

DcsInterface::DcsInterface(const std::string &rx_port, const std::string &tx_port, const std::string &ip_address)
    : dcs_socket_(rx_port, tx_port, ip_address)
{
}

void DcsInterface::register_dcs_monitor(const int dcs_id, const std::string &context)
{
    if (registered_contexts_map_.count(dcs_id) == 0)
    {
        registered_contexts_map_[dcs_id] = std::unordered_set<std::string>();
    }
    registered_contexts_map_[dcs_id].insert(context);
    active_contexts_map_[context] = dcs_id;
}

void DcsInterface::unregister_dcs_monitor(const std::string &context)
{
    if (active_contexts_map_.count(context) > 0)
    {
        const int registered_dcs_id = active_contexts_map_[context];
        registered_contexts_map_[registered_dcs_id].erase(context);
        if (registered_contexts_map_[registered_dcs_id].empty())
        {
            registered_contexts_map_.erase(registered_dcs_id);
        }
        active_contexts_map_.erase(context);
    }
}

std::vector<DcsIdValueUpdate> DcsInterface::get_next_dcs_update()
{
    // Receive next UDP message from DCS and strip header.
    const char header_delimiter = '*'; // Header content ends in a '*'.
    std::stringstream recv_msg = dcs_socket_.DcsReceive();

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

            // Push value update to vector if there are any contexts registered to it.
            if (registered_contexts_map_.count(dcs_id) > 0)
            {
                for (std::string context : registered_contexts_map_[dcs_id])
                {
                    DcsIdValueUpdate value_update;
                    value_update.context = context;
                    value_update.dcs_id = dcs_id;
                    value_update.dcs_id_value = reported_value;
                    received_updates.push_back(std::move(value_update));
                };
            }
        }
    }
    return received_updates;
}
