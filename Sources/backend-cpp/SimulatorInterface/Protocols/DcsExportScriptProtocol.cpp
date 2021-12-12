// Copyright 2020 Charles Tytler

#include "DcsExportScriptProtocol.h"

#include "Utilities/StringUtilities.h"

DcsExportScriptProtocol::DcsExportScriptProtocol(const SimulatorConnectionSettings &settings)
    : SimulatorInterface(settings)
{
    // Send a reset command on initialization by default.
    send_simulator_reset_command();
}

void DcsExportScriptProtocol::update_simulator_state()
{
    // Receive next UDP message from simulator and strip header.
    const char header_delimiter = '*'; // Header content ends in an '*'.
    std::stringstream recv_msg = simulator_socket_.receive_stream();

    std::string token;
    if (std::getline(recv_msg, token, header_delimiter)) {
        // Iterate through tokens received from single message.
        std::optional<std::pair<std::string, std::string>> maybe_token_pair;
        while (maybe_token_pair = pop_key_and_value(recv_msg, ':', '=')) {
            // Strip any trailing newline chars from value.
            auto ending_loc = maybe_token_pair.value().second.find_last_not_of('\n');
            std::string value = maybe_token_pair.value().second.substr(0, ending_loc + 1);
            handle_received_token(maybe_token_pair.value().first, value);
        }
    }
}

void DcsExportScriptProtocol::send_simulator_command(const std::string &address, const std::string &value)
{
    // Check that a valid address should be of the form "<device_id>,<button_id>"
    const auto address_as_components = split_pair(address, ',');
    const bool address_is_valid = (address_as_components && is_integer(address_as_components.value().first) &&
                                   is_integer(address_as_components.value().second));
    if (address_is_valid) {
        const std::string message_assembly = "C" + address + "," + value;
        simulator_socket_.send_string(message_assembly);
    }
}

void DcsExportScriptProtocol::send_simulator_reset_command() { simulator_socket_.send_string("R"); }

std::optional<std::string> DcsExportScriptProtocol::get_value_of_simulator_object_state(const int object_id) const
{
    if (current_game_state_by_dcs_id_.count(object_id) > 0) {
        if (!current_game_state_by_dcs_id_.at(object_id).empty()) {
            return current_game_state_by_dcs_id_.at(object_id);
        }
    }
    return std::nullopt;
}

std::optional<Decimal> DcsExportScriptProtocol::get_decimal_of_simulator_object_state(const int object_id) const
{
    if (current_game_state_by_dcs_id_.count(object_id) > 0) {
        if (is_number(current_game_state_by_dcs_id_.at(object_id))) {
            return current_game_state_by_dcs_id_.at(object_id);
        }
    }
    return std::nullopt;
}

void DcsExportScriptProtocol::clear_game_state() { current_game_state_by_dcs_id_.clear(); }

json DcsExportScriptProtocol::debug_get_current_game_state() const
{
    json current_game_state_printout;
    for (const auto &[key, value] : current_game_state_by_dcs_id_) {
        current_game_state_printout[std::to_string(key)] = value;
    }
    return current_game_state_printout;
}

void DcsExportScriptProtocol::handle_received_token(const std::string &key, const std::string &value)
{
    if (is_integer(key)) {
        current_game_state_by_dcs_id_.insert_or_assign(std::stoi(key), value);
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
