// Copyright 2020 Charles Tytler

#include "gtest/gtest.h"

#include "SimulatorInterface/SimulatorInterface.h"

namespace test
{

class SimulatorInterfaceDerivedClass : public SimulatorInterface
{
    using SimulatorInterface::SimulatorInterface;
    // Implement pure virtual functions to allow testing of common functions.
    void update_simulator_state(){};
    void send_simulator_command(const std::string &address, const std::string &value){};
    void send_simulator_reset_command(){};
    std::optional<std::string> get_value_of_simulator_object_state(const int object_id) const { return std::nullopt; }
    std::optional<Decimal> get_decimal_of_simulator_object_state(const int object_id) const { return std::nullopt; }
    json debug_get_current_game_state() const { return json{}; };
};

TEST(SimulatorInterfaceTest, invalid_connection_port_settings)
{
    const SimulatorConnectionSettings connection_settings = {"19ab", "abc", "127.0.0.1", ""};
    EXPECT_THROW(SimulatorInterfaceDerivedClass simulator_interface(connection_settings), std::runtime_error);
}

TEST(SimulatorInterfaceTest, udp_connect_on_construction_without_multicast)
{
    const SimulatorConnectionSettings connection_settings = {"1908", "1909", "127.0.0.1", ""};
    SimulatorInterfaceDerivedClass simulator_interface(connection_settings);
    // Expect no errors thrown.
    EXPECT_TRUE(simulator_interface.connection_settings_match(connection_settings));
}

TEST(SimulatorInterfaceTest, udp_connect_on_construction_with_multicast)
{
    const SimulatorConnectionSettings connection_settings = {"1908", "1909", "0.0.0.0", "239.255.50.10"};
    SimulatorInterfaceDerivedClass simulator_interface(connection_settings);
    // Expect no errors thrown.
    EXPECT_TRUE(simulator_interface.connection_settings_match(connection_settings));
}

TEST(SimulatorInterfaceTest, connection_settings_match_false)
{
    const SimulatorConnectionSettings connection_settings = {"1908", "1909", "127.0.0.1", ""};
    SimulatorInterfaceDerivedClass simulator_interface(connection_settings);
    SimulatorConnectionSettings mismatched_connection_settings = {"1111", "2222", "127.0.0.1", ""};
    EXPECT_FALSE(simulator_interface.connection_settings_match(mismatched_connection_settings));
}

TEST(SimulatorInterfaceTest, get_current_module_initial_value)
{
    SimulatorInterfaceDerivedClass simulator_interface(SimulatorConnectionSettings{"1908", "1909", "127.0.0.1", ""});
    // Test that the default value of an empty string is returned after initialization.
    EXPECT_EQ("", simulator_interface.get_current_simulator_module());
}

} // namespace test
