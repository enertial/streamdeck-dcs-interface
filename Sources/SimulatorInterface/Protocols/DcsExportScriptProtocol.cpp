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
    std::stringstream recv_msg = simulator_socket_.receive();

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
    // String should be of the form "<device_id>,<button_id>"
    if ((address.size() > 3) && (address.find(",") != std::string::npos)) {
        const std::string message_assembly = "C" + address + "," + value;
        simulator_socket_.send(message_assembly);
    }
}

void DcsExportScriptProtocol::send_simulator_reset_command() { simulator_socket_.send("R"); }

void DcsExportScriptProtocol::handle_received_token(const std::string &key, const std::string &value)
{
    if (is_integer(key)) {
        current_game_state_.insert_or_assign(std::stoi(key), value);
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
