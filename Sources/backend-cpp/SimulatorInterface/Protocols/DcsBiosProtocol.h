// Copyright 2020 Charles Tytler

#pragma once

#include "SimulatorInterface/Protocols/DcsBiosStreamParser.h"
#include "SimulatorInterface/SimulatorInterface.h"

class DcsBiosProtocol : public SimulatorInterface
{
  public:
    DcsBiosProtocol(const SimulatorConnectionSettings &settings);

    void update_simulator_state();

    void send_command(const std::string &control_reference, const std::string &value);

    void send_reset_command();

    std::optional<std::string> get_string_at_addr(const SimulatorAddress &address) const;

    std::optional<Decimal> get_value_at_addr(const SimulatorAddress &address) const;

    void clear_game_state();

    json get_current_state_as_json() const;

  private:
    /**
     * @brief Monitors and sets the current game module (aircraft name) from received data
     */
    void monitor_for_module_change();

    DcsBiosStreamParser protocol_parser_;

    // Maps received data to associated addresses.
    std::unordered_map<unsigned int, unsigned int> current_game_state_by_address_;
    std::vector<unsigned int> addresses_read_in_most_recent_frame_;

    // Default location of ACFT_NAME defined by MetaDataStart category of DCS BIOS json files.
    const SimulatorAddress ACFT_NAME_ADDRESS_{0x0000, 24};
};
