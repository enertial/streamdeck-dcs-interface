// Copyright 2020 Charles Tytler

#include "DcsBiosProtocol.h"

#include "Utilities/StringUtilities.h"

DcsBiosProtocol::DcsBiosProtocol(const SimulatorConnectionSettings &settings) : SimulatorInterface(settings)
{
    // Send a reset command on initialization by default.
    send_simulator_reset_command();
}

void DcsBiosProtocol::update_simulator_state()
{
    constexpr int MAX_UDP_MSG_SIZE = 1024; // Maximum UDP buffer size to read.
    char message_buffer[MAX_UDP_MSG_SIZE];
    const int message_size = simulator_socket_.receive_bytes(message_buffer, MAX_UDP_MSG_SIZE);
    // Read byte by byte.
    for (size_t i = 0; i < message_size; i++) {
        protocol_parser_.processByte(message_buffer[i], current_game_state_by_address_);
        if (protocol_parser_.at_end_of_frame()) {
            monitor_for_module_change();
        }
    }
}

void DcsBiosProtocol::send_simulator_command(const std::string &control_reference, const std::string &value)
{
    const std::string message_assembly = control_reference + " " + value + "\n";
    simulator_socket_.send_string(message_assembly);
}

void DcsBiosProtocol::send_simulator_reset_command() { simulator_socket_.send_string("SYNC E\n"); }

std::optional<std::string> DcsBiosProtocol::get_value_of_simulator_object_state(const SimulatorAddress &address) const
{
    if (address.type != AddressType::STRING) {
        return std::nullopt;
    }

    std::string assembled_string = "";
    for (unsigned int loc = address.address; loc < address.address + address.max_length; loc += 2) {
        if (current_game_state_by_address_.count(address.address) > 0) {
            // Convert 16-bit data at current location to 2 characters.
            char characters[2];
            memcpy(characters, &current_game_state_by_address_.at(loc), sizeof(characters));
            // Append characters to string, returning once encountering a null character.
            for (char character : characters) {
                if (character == '\0') {
                    return assembled_string;
                } else {
                    assembled_string += character;
                }
            }
        }
    }
    return std::nullopt;
}

std::optional<Decimal> DcsBiosProtocol::get_decimal_of_simulator_object_state(const SimulatorAddress &address) const
{
    if (address.type == AddressType::INTEGER && current_game_state_by_address_.count(address.address) > 0) {
        return Decimal((current_game_state_by_address_.at(address.address) & address.mask) >> address.shift, 0);
    }
    return std::nullopt;
}

void DcsBiosProtocol::clear_game_state()
{
    current_game_state_by_address_.clear();
    current_game_module_ = "";
}

json DcsBiosProtocol::debug_get_current_game_state() const
{
    json current_game_state_printout;
    for (const auto &[key, value] : current_game_state_by_address_) {
        current_game_state_printout[std::to_string(key)] = value;
    }
    return current_game_state_printout;
}

void DcsBiosProtocol::monitor_for_module_change()
{
    const auto maybe_aircraft_name = get_value_of_simulator_object_state(ACFT_NAME_ADDRESS_);
    if (maybe_aircraft_name) {
        if (maybe_aircraft_name.value() != current_game_module_) {
            // Clear game state when and reset to only data received in the most recent frame when new active aircraft
            // module is detected.
            current_game_state_by_address_ = protocol_parser_.get_data_by_address_updated_this_frame();
            current_game_module_ = maybe_aircraft_name.value();
        }
    }
}