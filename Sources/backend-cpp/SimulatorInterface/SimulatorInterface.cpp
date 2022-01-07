// Copyright 2020 Charles Tytler

#include "SimulatorInterface.h"

#include "Utilities/StringUtilities.h"

SimulatorInterface::SimulatorInterface(const SimulatorConnectionSettings &settings)
    : simulator_socket_(settings.ip_address, settings.rx_port, settings.tx_port, settings.multicast_address),
      connection_settings_(settings)
{
}

bool SimulatorInterface::connection_settings_match(const SimulatorConnectionSettings &settings) const
{
    return ((settings.rx_port == connection_settings_.rx_port) && (settings.tx_port == connection_settings_.tx_port) &&
            (settings.ip_address == connection_settings_.ip_address) &&
            (settings.multicast_address == connection_settings_.multicast_address));
}

std::string SimulatorInterface::get_current_simulator_module() const { return current_game_module_; }
