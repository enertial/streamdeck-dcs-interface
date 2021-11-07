// Copyright 2020 Charles Tytler

#include "DcsBiosProtocol.h"

#include "Utilities/StringUtilities.h"

DcsBiosProtocol::DcsBiosProtocol(const SimulatorConnectionSettings &settings) : SimulatorInterface(settings)
{
    // Send a reset command on initialization by default.
    send_simulator_reset_command();
}

void DcsBiosProtocol::update_simulator_state() {}

void DcsBiosProtocol::send_simulator_command(const std::string &address, const std::string &value)
{
    const std::string message_assembly = address + " " + value + "\n";
    simulator_socket_.send(message_assembly);
}

void DcsBiosProtocol::send_simulator_reset_command() { simulator_socket_.send("SYNC E"); }
