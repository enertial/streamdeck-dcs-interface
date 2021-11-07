// Copyright 2020 Charles Tytler

#include "DcsBiosInterface.h"

#include "Utilities/StringUtilities.h"

DcsBiosInterface::DcsBiosInterface(const SimulatorConnectionSettings &settings) : SimulatorInterface(settings)
{
    // Send a reset command on initialization by default.
    send_simulator_reset_command();
}

void DcsBiosInterface::update_simulator_state() {}

void DcsBiosInterface::send_simulator_command(const std::string &address, const std::string &value)
{
    const std::string message_assembly = address + " " + value + "\n";
    simulator_socket_.send(message_assembly);
}

void DcsBiosInterface::send_simulator_reset_command() { simulator_socket_.send("SYNC E"); }
