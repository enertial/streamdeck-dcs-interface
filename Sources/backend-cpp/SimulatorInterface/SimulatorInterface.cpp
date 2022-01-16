// Copyright 2020 Charles Tytler

#include "SimulatorInterface.h"

#include "Utilities/StringUtilities.h"

SimulatorAddress::SimulatorAddress(unsigned int address) : type(AddressType::ADDRESS_ONLY), address(address) {}
SimulatorAddress::SimulatorAddress(unsigned int address, unsigned int mask, uint8_t shift)
    : type(AddressType::INTEGER), address(address), mask(mask), shift(shift)
{
}
SimulatorAddress::SimulatorAddress(unsigned int address, unsigned int max_length)
    : type(AddressType::STRING), address(address), max_length(max_length)
{
}

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

std::string SimulatorInterface::get_current_module() const { return current_game_module_; }
