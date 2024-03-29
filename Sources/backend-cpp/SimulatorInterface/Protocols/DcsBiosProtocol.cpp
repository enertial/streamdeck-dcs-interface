// Copyright 2020 Charles Tytler

#include "DcsBiosProtocol.h"

#include "Utilities/StringUtilities.h"

DcsBiosProtocol::DcsBiosProtocol(const SimulatorConnectionSettings &settings) : SimulatorInterface(settings)
{
    // Send a reset command on initialization by default.
    send_reset_command();
}

void DcsBiosProtocol::update_simulator_state()
{
    constexpr int MAX_UDP_MSG_SIZE = 1024; // Maximum UDP buffer size to read.
    char message_buffer[MAX_UDP_MSG_SIZE];
    const int message_size = simulator_socket_.receive_bytes(message_buffer, MAX_UDP_MSG_SIZE);
    // Read byte by byte.
    for (int i = 0; i < message_size; i++) {
        protocol_parser_.processByte(message_buffer[i], current_game_state_by_address_);
        if (protocol_parser_.at_end_of_frame()) {
            monitor_for_module_change();
        }
    }
}

void DcsBiosProtocol::send_command(const std::string &control_reference, const std::string &value)
{
    const std::string message_assembly = control_reference + " " + value + "\n";
    simulator_socket_.send_string(message_assembly);
}

void DcsBiosProtocol::send_reset_command() { simulator_socket_.send_string("SYNC E\n"); }

std::optional<std::string> DcsBiosProtocol::get_string_at_addr(const SimulatorAddress &address) const
{
    const bool data_exists_at_start_address = current_game_state_by_address_.count(address.address) > 0;
    if (address.type == AddressType::ADDRESS_ONLY || !data_exists_at_start_address) {
        return std::nullopt;
    }

    if (address.type == AddressType::INTEGER) {
        return std::to_string((current_game_state_by_address_.at(address.address) & address.mask) >> address.shift);
    }

    std::string assembled_string = "";
    for (unsigned int loc = address.address; loc < address.address + address.max_length; loc += 2) {
        if (current_game_state_by_address_.count(loc) > 0) {
            // Convert 16-bit data at current location to 2 characters.
            char characters[2];
            memcpy(characters, &current_game_state_by_address_.at(loc), sizeof(characters));
            // Append characters to string, returning early if encountering a null character.
            for (char character : characters) {
                if (character == '\0') {
                    return assembled_string;
                } else {
                    assembled_string += character;
                }
            }
        }
    }
    return assembled_string;
}

std::optional<Decimal> DcsBiosProtocol::get_value_at_addr(const SimulatorAddress &address) const
{
    if (address.type == AddressType::INTEGER && current_game_state_by_address_.count(address.address) > 0) {
        return Decimal((current_game_state_by_address_.at(address.address) & address.mask) >> address.shift);
    }
    return std::nullopt;
}

void DcsBiosProtocol::clear_game_state()
{
    current_game_state_by_address_.clear();
    current_module_ = "";
}

json DcsBiosProtocol::get_current_state_as_json() const
{
    json current_game_state_printout;
    for (const auto &[key, value] : current_game_state_by_address_) {
        current_game_state_printout[std::to_string(key)] = value;
    }
    return current_game_state_printout;
}

void DcsBiosProtocol::monitor_for_module_change()
{
    const auto maybe_aircraft_name = get_string_at_addr(ACFT_NAME_ADDRESS_);
    if (maybe_aircraft_name) {
        if (maybe_aircraft_name.value() != current_module_) {
            // Clear game state when and reset to only data received in the most recent frame when new active aircraft
            // module is detected.
            current_game_state_by_address_ = protocol_parser_.get_data_by_address_updated_this_frame();
            current_module_ = maybe_aircraft_name.value();
        }
    }
}