// Copyright 2020 Charles Tytler

#pragma once

#include "SimulatorInterface/SimulatorInterface.h"

class DcsExportScriptProtocol : public SimulatorInterface
{
  public:
    DcsExportScriptProtocol(const SimulatorConnectionSettings &settings);

    void update_simulator_state();

    void send_simulator_command(const std::string &address, const std::string &value);

    void send_simulator_reset_command();

    std::optional<std::string> get_value_of_simulator_object_state(const SimulatorAddress &address) const;

    std::optional<Decimal> get_decimal_of_simulator_object_state(const SimulatorAddress &address) const;

    void clear_game_state();

    json debug_get_current_game_state() const;

  private:
    /**
     * @brief Processes received tokens of simulator game updates.
     * @param key Key for updated value
     * @param value Updated value.
     */
    void handle_received_token(const std::string &key, const std::string &value);

    // Maps object ID keys of received values to their most recently published values.
    std::unordered_map<int, std::string> current_game_state_by_dcs_id_;
};
