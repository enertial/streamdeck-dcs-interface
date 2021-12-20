// Copyright 2020 Charles Tytler

#pragma once

#include "SimulatorInterface/Protocols/DcsBiosStreamParser.h"
#include "SimulatorInterface/SimulatorInterface.h"

class DcsBiosProtocol : public SimulatorInterface
{
  public:
    DcsBiosProtocol(const SimulatorConnectionSettings &settings);

    void update_simulator_state();

    void send_simulator_command(const std::string &control_reference, const std::string &value);

    void send_simulator_reset_command();

    std::optional<std::string> get_value_of_simulator_object_state(const SimulatorAddress &address) const;

    std::optional<Decimal> get_decimal_of_simulator_object_state(const SimulatorAddress &address) const;

    void clear_game_state();

    json debug_get_current_game_state() const;

  private:
    /**
     * @brief Sets the current game module (aircraft name) from received data
     */
    void set_current_game_module();

    DcsBiosStreamParser protocol_parser_;

    // Maps received data to associated addresses.
    std::unordered_map<unsigned int, unsigned int> current_game_state_by_address_;
    std::vector<unsigned int> addresses_read_in_most_recent_frame_;

    // Default location of ACFT_NAME defined by MetaDataStart category of DCS BIOS json files.
    const SimulatorAddress ACFT_NAME_ADDRESS_{0x0000, 24};
};
