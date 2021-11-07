// Copyright 2020 Charles Tytler

#include "SimulatorInterface.h"

#include "Utilities/StringUtilities.h"

SimulatorInterface::SimulatorInterface(const SimulatorConnectionSettings &settings)
    : simulator_socket_(settings.ip_address, settings.rx_port, settings.tx_port, settings.multicast_address),
      connection_settings_(settings)
{
}

bool SimulatorInterface::connection_settings_match(const SimulatorConnectionSettings &settings)
{
    return ((settings.rx_port == connection_settings_.rx_port) && (settings.tx_port == connection_settings_.tx_port) &&
            (settings.ip_address == connection_settings_.ip_address) &&
            (settings.multicast_address == connection_settings_.multicast_address));
}

std::string SimulatorInterface::get_current_simulator_module() { return current_game_module_; }

std::optional<std::string> SimulatorInterface::get_value_of_simulator_object_state(const int object_id)
{
    if (current_game_state_.count(object_id) > 0) {
        if (!current_game_state_[object_id].empty()) {
            return current_game_state_[object_id];
        }
    }
    return std::nullopt;
}

std::optional<Decimal> SimulatorInterface::get_decimal_of_simulator_object_state(const int object_id)
{
    if (current_game_state_.count(object_id) > 0) {
        if (is_number(current_game_state_[object_id])) {
            return current_game_state_[object_id];
        }
    }
    return std::nullopt;
}

void SimulatorInterface::clear_game_state() { current_game_state_.clear(); }

std::map<int, std::string> SimulatorInterface::debug_get_current_game_state() { return current_game_state_; }
