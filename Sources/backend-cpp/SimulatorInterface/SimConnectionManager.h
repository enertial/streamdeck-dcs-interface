// Copyright 2022 Charles Tytler

#pragma once

#include "SimulatorInterface/SimulatorInterface.h"

#include <unordered_map>

enum class Protocol { DCS_BIOS, DCS_ExportScript };

class SimConnectionManager
{
  public:
    SimConnectionManager() = default;

    /**
     * @brief Get connection status of protocol.
     */
    bool is_connected(const Protocol protocol);
    bool is_connected_with_settings(const Protocol protocol, const SimulatorConnectionSettings &settings);

    /**
     * @brief Connect to protocol with specified settings. Will overwrite any existing connection to that protocol.
     */
    void connect_to_protocol(const Protocol protocol, const SimulatorConnectionSettings &settings);

    /**
     * @brief Disconnect interface for specified protocol.
     */
    void disconnect_protocol(const Protocol protocol);

    /**
     * @brief Get reference to the simulator interface for specific protocol.
     */
    SimulatorInterface *get_interface(const Protocol protocol);

    /**
     * @brief Call update state function for all connected protocols.
     */
    void update_all();

  private:
    std::unordered_map<Protocol, std::unique_ptr<SimulatorInterface>> simulator_interfaces_;
};
