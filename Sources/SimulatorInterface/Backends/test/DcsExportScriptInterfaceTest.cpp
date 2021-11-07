// Copyright 2020 Charles Tytler

#include "gtest/gtest.h"

#include "SimulatorInterface/Backends/DcsExportScriptInterface.h"

namespace test
{
TEST(DcsExportScriptInterfaceTest, invalid_connection_port_settings)
{
    SimulatorConnectionSettings connection_settings = {"19ab", "abc", "127.0.0.1", ""};
    EXPECT_THROW(DcsExportScriptInterface simulator_interface(connection_settings), std::runtime_error);
}

TEST(DcsExportScriptInterfaceTest, dcs_reset_command_on_construction)
{
    SimulatorConnectionSettings connection_settings = {"1908", "1909", "127.0.0.1", ""};
    UdpSocket mock_dcs(connection_settings.ip_address, connection_settings.tx_port, connection_settings.rx_port);
    DcsExportScriptInterface simulator_interface(connection_settings);

    // Test that the reset message "R" is received by DCS on creation of DcsExportScriptInterface.
    std::stringstream ss = mock_dcs.receive();
    EXPECT_EQ("R", ss.str());
}

class DcsExportScriptInterfaceTestFixture : public ::testing::Test
{
  public:
    DcsExportScriptInterfaceTestFixture()
        : // Mock DCS socket uses the reverse rx and tx ports of simulator_interface so it can communicate with it.
          mock_dcs(connection_settings.ip_address, connection_settings.tx_port, connection_settings.rx_port),
          simulator_interface(connection_settings)
    {
        // Consume intial reset command sent to to mock_dcs.
        (void)mock_dcs.receive();
    }

    SimulatorConnectionSettings connection_settings = {"1908", "1909", "127.0.0.1", ""};
    UdpSocket mock_dcs;                           // A socket that will mock Send/Receive messages from DCS.
    DcsExportScriptInterface simulator_interface; // Simulator Interface to test.
};

TEST_F(DcsExportScriptInterfaceTestFixture, empty_game_state_on_initialization)
{
    // Test that current game state initializes as empty.
    std::unordered_map<int, std::string> current_game_state = simulator_interface.debug_get_current_game_state();
    EXPECT_EQ(0, current_game_state.size());
}

TEST_F(DcsExportScriptInterfaceTestFixture, update_simulator_state)
{
    // TEST 1 - Received values are stored and retrievable.
    // Send a single message from mock DCS that contains updates for multiple IDs.
    std::string mock_dcs_message = "header*761=1:765=2.00:2026=TEXT_STR:2027=4";
    mock_dcs.send(mock_dcs_message);
    simulator_interface.update_simulator_state();

    // Query for ID updates.
    EXPECT_EQ("1", simulator_interface.get_value_of_simulator_object_state(761).value());
    EXPECT_EQ("2.00", simulator_interface.get_value_of_simulator_object_state(765).value());
    EXPECT_EQ("TEXT_STR", simulator_interface.get_value_of_simulator_object_state(2026).value());
    EXPECT_EQ("4", simulator_interface.get_value_of_simulator_object_state(2027).value());

    // TEST 2 - Received values will overwrite their previous values.
    // Send a new message with one ID value updated.
    mock_dcs_message = "header*765=99.99";
    mock_dcs.send(mock_dcs_message);
    simulator_interface.update_simulator_state();

    // Query for ID updates - Test that only the value for ID 765 has changed.
    EXPECT_EQ("99.99", simulator_interface.get_value_of_simulator_object_state(765).value());

    EXPECT_EQ("1", simulator_interface.get_value_of_simulator_object_state(761).value());
    EXPECT_EQ("TEXT_STR", simulator_interface.get_value_of_simulator_object_state(2026).value());
    EXPECT_EQ("4", simulator_interface.get_value_of_simulator_object_state(2027).value());
}

TEST_F(DcsExportScriptInterfaceTestFixture, update_simulator_state_handle_newline_chars)
{
    // Send a single message from mock DCS that contains newline characters at the end of tokens.
    std::string mock_dcs_message = "header*761=1\n:765=2.00\n:2026=TEXT_STR\n:2027=4\n";
    mock_dcs.send(mock_dcs_message);
    simulator_interface.update_simulator_state();

    // Query for ID updates.
    EXPECT_EQ("1", simulator_interface.get_value_of_simulator_object_state(761).value());
    EXPECT_EQ("2.00", simulator_interface.get_value_of_simulator_object_state(765).value());
    EXPECT_EQ("TEXT_STR", simulator_interface.get_value_of_simulator_object_state(2026).value());
    EXPECT_EQ("4", simulator_interface.get_value_of_simulator_object_state(2027).value());
}

TEST_F(DcsExportScriptInterfaceTestFixture, update_simulator_state_end_of_mission)
{
    // Send a single message from mock DCS that contains updates for multiple IDs.
    // Expect that the number of stored values in game state is non-zero.
    std::string mock_dcs_message = "header*761=1:765=2.00:2026=TEXT_STR:2027=4";
    mock_dcs.send(mock_dcs_message);
    simulator_interface.update_simulator_state();
    std::unordered_map<int, std::string> current_game_state = simulator_interface.debug_get_current_game_state();
    EXPECT_TRUE(current_game_state.size() > 0);

    // Test that game state is cleared when a "STOP" message is received, signifying end of mission.
    mock_dcs_message = "header*Ikarus=stop";
    mock_dcs.send(mock_dcs_message);
    simulator_interface.update_simulator_state();
    current_game_state = simulator_interface.debug_get_current_game_state();
    EXPECT_EQ(0, current_game_state.size());
}

TEST_F(DcsExportScriptInterfaceTestFixture, send_simulator_command)
{
    const int button_id = 3250;
    const std::string device_id = "24";
    const std::string value = "1";
    // Message is sent prepended with "C".
    const std::string expected_msg_buffer = "C24,3250,1";

    simulator_interface.send_simulator_command(button_id, device_id, value);
    std::stringstream ss_received = mock_dcs.receive();
    EXPECT_EQ(ss_received.str(), expected_msg_buffer);
}

TEST_F(DcsExportScriptInterfaceTestFixture, send_simulator_reset_command)
{
    simulator_interface.send_simulator_reset_command();
    std::stringstream ss_received = mock_dcs.receive();
    EXPECT_EQ(ss_received.str(), "R");
}

TEST_F(DcsExportScriptInterfaceTestFixture, get_current_simulator_module_init)
{
    // Test that the default value of an empty string is returned after initialization.
    EXPECT_EQ("", simulator_interface.get_current_simulator_module());
}

TEST_F(DcsExportScriptInterfaceTestFixture, get_current_simulator_module)
{
    // Test that game module is updated when the "File" message with module name is received.
    std::string mock_dcs_message = "header*File=AV8BNA";
    mock_dcs.send(mock_dcs_message);
    simulator_interface.update_simulator_state();
    EXPECT_EQ("AV8BNA", simulator_interface.get_current_simulator_module());

    // Test that game module is cleared when a "STOP" message is received, signifying end of mission.
    mock_dcs_message = "header*Ikarus=stop";
    mock_dcs.send(mock_dcs_message);
    simulator_interface.update_simulator_state();
    EXPECT_EQ("", simulator_interface.get_current_simulator_module());
}

TEST_F(DcsExportScriptInterfaceTestFixture, get_value_of_simulator_object_state_if_nonexistant)
{
    EXPECT_FALSE(simulator_interface.get_value_of_simulator_object_state(999));
}

TEST_F(DcsExportScriptInterfaceTestFixture, get_value_of_simulator_object_state_with_valid_value)
{
    std::string mock_dcs_message = "header*765=99.99";
    mock_dcs.send(mock_dcs_message);
    simulator_interface.update_simulator_state();

    auto maybe_value = simulator_interface.get_value_of_simulator_object_state(765);
    EXPECT_TRUE(maybe_value);
    EXPECT_EQ(maybe_value.value(), "99.99");
}

TEST_F(DcsExportScriptInterfaceTestFixture, get_value_of_simulator_object_state_if_empty)
{
    std::string mock_dcs_message = "header*765=";
    mock_dcs.send(mock_dcs_message);
    simulator_interface.update_simulator_state();

    auto maybe_value = simulator_interface.get_value_of_simulator_object_state(765);
    EXPECT_FALSE(maybe_value);
}

TEST_F(DcsExportScriptInterfaceTestFixture, get_decimal_of_simulator_object_state_if_nonexistant)
{
    EXPECT_FALSE(simulator_interface.get_decimal_of_simulator_object_state(999));
}

TEST_F(DcsExportScriptInterfaceTestFixture, get_decimal_of_simulator_object_state_with_valid_value)
{
    std::string mock_dcs_message = "header*765=99.99";
    mock_dcs.send(mock_dcs_message);
    simulator_interface.update_simulator_state();

    auto maybe_value = simulator_interface.get_decimal_of_simulator_object_state(765);
    EXPECT_TRUE(maybe_value);
    EXPECT_EQ(maybe_value.value(), Decimal("99.99"));
}

TEST_F(DcsExportScriptInterfaceTestFixture, get_decimal_of_simulator_object_state_with_non_decimal_value)
{
    std::string mock_dcs_message = "header*765=STRING";
    mock_dcs.send(mock_dcs_message);
    simulator_interface.update_simulator_state();

    auto maybe_value = simulator_interface.get_decimal_of_simulator_object_state(765);
    EXPECT_FALSE(maybe_value);
}

TEST_F(DcsExportScriptInterfaceTestFixture, get_decimal_of_simulator_object_state_if_empty)
{
    std::string mock_dcs_message = "header*765=";
    mock_dcs.send(mock_dcs_message);
    simulator_interface.update_simulator_state();

    auto maybe_value = simulator_interface.get_value_of_simulator_object_state(765);
    EXPECT_FALSE(maybe_value);
}

TEST_F(DcsExportScriptInterfaceTestFixture, clear_game_state)
{
    // Send a single message from mock DCS that contains updates for multiple IDs.
    // Expect that the number of stored values in game state is non-zero.
    std::string mock_dcs_message = "header*761=1:765=2.00:2026=TEXT_STR:2027=4";
    mock_dcs.send(mock_dcs_message);
    simulator_interface.update_simulator_state();
    std::unordered_map<int, std::string> current_game_state = simulator_interface.debug_get_current_game_state();
    EXPECT_TRUE(current_game_state.size() > 0);

    // Test that game state is able to be cleared.
    simulator_interface.clear_game_state();
    current_game_state = simulator_interface.debug_get_current_game_state();
    EXPECT_EQ(0, current_game_state.size());
}

TEST_F(DcsExportScriptInterfaceTestFixture, debug_print_format)
{
    // Send a single message from mock DCS that contains updates for multiple IDs.
    // Expect that the number of stored values in game state is non-zero.
    std::string mock_dcs_message = "header*761=1:765=2.00:2026=TEXT_STR:2027=4";
    mock_dcs.send(mock_dcs_message);
    simulator_interface.update_simulator_state();
    std::unordered_map<int, std::string> current_game_state = simulator_interface.debug_get_current_game_state();

    EXPECT_EQ(current_game_state[761], "1");
    EXPECT_EQ(current_game_state[765], "2.00");
    EXPECT_EQ(current_game_state[2026], "TEXT_STR");
    EXPECT_EQ(current_game_state[2027], "4");
}
} // namespace test
