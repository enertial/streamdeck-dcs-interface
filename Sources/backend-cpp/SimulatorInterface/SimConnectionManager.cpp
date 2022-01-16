// Copyright 2022 Charles Tytler

#include "SimConnectionManager.h"

#include "SimulatorInterface/Protocols/DcsBiosProtocol.h"
#include "SimulatorInterface/Protocols/DcsExportScriptProtocol.h"

bool SimConnectionManager::is_connected(const Protocol protocol) { return simulator_interfaces_.count(protocol) > 0; }

bool SimConnectionManager::is_connected_with_settings(const Protocol protocol,
                                                      const SimulatorConnectionSettings &settings)
{
    return is_connected(protocol) && simulator_interfaces_[protocol]->connection_settings_match(settings);
}

void SimConnectionManager::connect_to_protocol(const Protocol protocol, const SimulatorConnectionSettings &settings)
{
    switch (protocol) {
    case Protocol::DCS_BIOS:
        simulator_interfaces_[protocol] = std::make_unique<DcsBiosProtocol>(settings);
        break;
    case Protocol::DCS_ExportScript:
        simulator_interfaces_[protocol] = std::make_unique<DcsExportScriptProtocol>(settings);
        break;
    }
}

void SimConnectionManager::disconnect_protocol(const Protocol protocol) { simulator_interfaces_.erase(protocol); }

SimulatorInterface *SimConnectionManager::get_interface(const Protocol protocol)
{
    return simulator_interfaces_[protocol].get();
}

void SimConnectionManager::update_all()
{
    for (const auto &elem : simulator_interfaces_) {
        elem.second->update_simulator_state();
    }
}
