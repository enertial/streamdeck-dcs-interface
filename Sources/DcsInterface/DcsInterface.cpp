// Copyright 2020 Charles Tytler

#include "pch.h"

#include "DcsInterface.h"
#include "StringUtilities.h"

DcsInterface::DcsInterface(const DcsConnectionSettings &settings)
    : dcs_socket_(settings.rx_port, settings.tx_port, settings.ip_address), connection_settings_(settings) {
    // Send a reset to request a resend of data in case DCS mission is already running.
    send_dcs_reset_command();
}

bool DcsInterface::connection_settings_match(const DcsConnectionSettings &settings) {
    return ((settings.rx_port == connection_settings_.rx_port) && (settings.tx_port == connection_settings_.tx_port) &&
            (settings.ip_address == connection_settings_.ip_address));
}

void DcsInterface::update_dcs_state() {
    // Receive next UDP message from DCS and strip header.
    const char header_delimiter = '*'; // Header content ends in an '*'.
    std::stringstream recv_msg = dcs_socket_.DcsReceive();

    std::string token;
    if (std::getline(recv_msg, token, header_delimiter)) {
        // Iterate through tokens received from single message.
        std::pair<std::string, std::string> key_and_value;
        while (pop_key_and_value(recv_msg, ':', '=', key_and_value)) {
            // Strip any trailing newline chars from value.
            auto value_end_loc = key_and_value.second.find_last_not_of('\n');
            std::string value = key_and_value.second.substr(0, value_end_loc + 1);
            handle_received_token(key_and_value.first, value);
        }
    }
}

std::string DcsInterface::get_current_dcs_module() { return current_game_module_; }

std::string DcsInterface::get_value_of_dcs_id(const int dcs_id) {
    if (current_game_state_.count(dcs_id) > 0) {
        return current_game_state_[dcs_id];
    } else {
        return "";
    }
}

void DcsInterface::send_dcs_command(const int button_id, const std::string &device_id, const std::string &value) {
    const std::string message_assembly = "C" + device_id + "," + std::to_string(3000 + button_id) + "," + value;
    dcs_socket_.DcsSend(message_assembly);
}

void DcsInterface::send_dcs_reset_command() { dcs_socket_.DcsSend("R"); }

void DcsInterface::clear_game_state() { current_game_state_.clear(); }

std::map<int, std::string> DcsInterface::debug_get_current_game_state() { return current_game_state_; }

void DcsInterface::handle_received_token(const std::string &key, const std::string &value) {
    if (is_integer(key)) {
        current_game_state_[std::stoi(key)] = value;
    } else if (key == "File") {
        current_game_module_ = value;
    } else if (key == "Ikarus" || key == "DAC" || key == "DCS") {
        // Stop is received when user has quit mission -- game state should be cleared.
        if (value == "stop") {
            clear_game_state();
            current_game_module_ = "";
        }
    }
}
