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
        std::pair<std::string, std::string> key_and_value;
        while (pop_key_and_value(recv_msg, ':', '=', key_and_value)) {
            // Strip any trailing newline chars from value.
            auto value_end_loc = key_and_value.second.find_last_not_of('\n');
            std::string value = key_and_value.second.substr(0, value_end_loc + 1);
            handle_received_token(key_and_value.first, value);
        }
    }
}

void DcsExportScriptProtocol::send_simulator_command(const int button_id,
                                                     const std::string &device_id,
                                                     const std::string &value)
{
    const std::string message_assembly = "C" + device_id + "," + std::to_string(button_id) + "," + value;
    simulator_socket_.send(message_assembly);
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
