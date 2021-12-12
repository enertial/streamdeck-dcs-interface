// Copyright 2020 Charles Tytler

#include "gtest/gtest.h"

#include "SimulatorInterface/Protocols/DcsBiosProtocol.h"

namespace test
{
TEST(DcsBiosProtocolTest, invalid_connection_port_settings)
{
    SimulatorConnectionSettings connection_settings = {"19ab", "abc", "127.0.0.1", ""};
    EXPECT_THROW(DcsBiosProtocol simulator_interface(connection_settings), std::runtime_error);
}

TEST(DcsBiosProtocolTest, dcs_reset_command_on_construction)
{
    SimulatorConnectionSettings connection_settings = {"1908", "1909", "127.0.0.1", ""};
    UdpSocket mock_dcs(connection_settings.ip_address, connection_settings.tx_port, connection_settings.rx_port);
    DcsBiosProtocol simulator_interface(connection_settings);

    // Test that the reset message "R" is received by DCS on creation of DcsBiosProtocol.
    std::stringstream ss = mock_dcs.receive_stream();
    EXPECT_EQ("SYNC E\n", ss.str());
}

class DcsBiosProtocolTestFixture : public ::testing::Test
{
  public:
    DcsBiosProtocolTestFixture()
        : // Mock DCS socket uses the reverse rx and tx ports of simulator_interface so it can communicate with it.
          mock_dcs(connection_settings.ip_address, connection_settings.tx_port, connection_settings.rx_port),
          simulator_interface(connection_settings)
    {
        // Consume intial reset command sent to to mock_dcs.
        (void)mock_dcs.receive_stream();
    }

    SimulatorConnectionSettings connection_settings = {"1908", "1909", "127.0.0.1", ""};
    UdpSocket mock_dcs;                  // A socket that will mock Send/Receive messages from DCS.
    DcsBiosProtocol simulator_interface; // Simulator Interface to test.
};

TEST_F(DcsBiosProtocolTestFixture, get_current_simulator_module_init)
{
    // Test that the default value of an empty string is returned after initialization.
    EXPECT_EQ("", simulator_interface.get_current_simulator_module());
}

TEST_F(DcsBiosProtocolTestFixture, update_simulator_state_overwrite_values)
{
    // TEST 1 - Received values are stored and retrievable.
    // Send a single message from mock DCS that contains updates for multiple IDs.
    const unsigned char mock_dcs_bios_message_one[] = {0x55, 0x55, 0x55, 0x55,                         //
                                                       0x08, 0x00, 0x02, 0x00, 0x6F, 0x72,             //
                                                       0x02, 0x04, 0x04, 0x00, 0x31, 0x30, 0x2E, 0x30, //
                                                       0x0C, 0x74, 0x02, 0x00, 0x00, 0x02};
    // mock_dcs.send_string(mock_dcs_message_one);
    simulator_interface.update_simulator_state();

    // Query for ID updates.
    EXPECT_TRUE(true); // TODO.

    // TEST 2 - Received values will overwrite their previous values.
    // Send a new message with one ID value updated.
    const unsigned char mock_dcs_bios_message_two[] = {0x55, 0x55, 0x55, 0x55, 0x0C, 0x74, 0x02, 0x00, 0x00, 0x00};
    // mock_dcs.send_string(mock_dcs_message_two);
    simulator_interface.update_simulator_state();

    // Query for ID updates - Test that only the value for address x740C has changed.
    EXPECT_TRUE(true); // TODO.
}

TEST_F(DcsBiosProtocolTestFixture, update_simulator_state_handle_incomplete_message)
{
    // Send a single message from mock DCS that contains updates for multiple IDs.
    const unsigned char mock_dcs_bios_message_one[] = {0x55, 0x55, 0x55, 0x55,                         //
                                                       0x08, 0x00, 0x02, 0x00, 0x6F, 0x72,             //
                                                       0x02, 0x04, 0x04, 0x00, 0x31, 0x30, 0x2E, 0x30, //
                                                       0x0C, 0x74, 0x02, 0x00};
    // mock_dcs.send_string(mock_dcs_message_one);
    simulator_interface.update_simulator_state();

    // Query for ID updates.
    EXPECT_TRUE(true); // TODO.
}

TEST_F(DcsBiosProtocolTestFixture, update_simulator_state_handle_start_of_mission)
{
    ASSERT_EQ("", simulator_interface.get_current_simulator_module());
    // Send a single message from mock DCS that contains updates for multiple IDs.
    const unsigned char mock_dcs_bios_message_one[] = {0x55, 0x55, 0x55, 0x55,                         //
                                                       0x08, 0x00, 0x02, 0x00, 0x6F, 0x72,             //
                                                       0x02, 0x04, 0x04, 0x00, 0x31, 0x30, 0x2E, 0x30, //
                                                       0x0C, 0x74, 0x02, 0x00};
    // mock_dcs.send_string(mock_dcs_message_one);
    simulator_interface.update_simulator_state();

    // Query for ID updates.
    // Test that game module is updated.
    EXPECT_TRUE(true); // TODO.
}
TEST_F(DcsBiosProtocolTestFixture, update_simulator_state_handle_end_of_mission)
{
    // Send a single message from mock DCS that contains updates for multiple IDs.
    const unsigned char mock_dcs_bios_message_one[] = {0x55, 0x55, 0x55, 0x55,                         //
                                                       0x08, 0x00, 0x02, 0x00, 0x6F, 0x72,             //
                                                       0x02, 0x04, 0x04, 0x00, 0x31, 0x30, 0x2E, 0x30, //
                                                       0x0C, 0x74, 0x02, 0x00};
    // mock_dcs.send_string(mock_dcs_message_one);
    simulator_interface.update_simulator_state();

    // Query for ID updates.
    EXPECT_TRUE(true); // TODO.
    // Test that game module is cleared.
}

TEST_F(DcsBiosProtocolTestFixture, send_simulator_command)
{
    const std::string address = "BIOS_HANDLE";
    const std::string value = "1";
    const std::string expected_msg_buffer = "BIOS_HANDLE 1\n";

    simulator_interface.send_simulator_command(address, value);
    std::stringstream ss_received = mock_dcs.receive_stream();
    EXPECT_EQ(ss_received.str(), expected_msg_buffer);
}

TEST_F(DcsBiosProtocolTestFixture, send_simulator_reset_command)
{
    simulator_interface.send_simulator_reset_command();
    std::stringstream ss_received = mock_dcs.receive_stream();
    EXPECT_EQ(ss_received.str(), "SYNC E\n");
}

TEST_F(DcsBiosProtocolTestFixture, get_value_of_simulator_object_state_if_nonexistant)
{
    EXPECT_FALSE(simulator_interface.get_value_of_simulator_object_state(999));
}

TEST_F(DcsBiosProtocolTestFixture, get_value_of_simulator_object_state_with_valid_value)
{
    const unsigned char mock_dcs_bios_message[] = {0x55, 0x55, 0x55, 0x55, 0x08, 0x00, 0x02, 0x00, 0x01, 0x00};
    // mock_dcs.send_string(mock_dcs_message);
    simulator_interface.update_simulator_state();

    // auto maybe_value = simulator_interface.get_value_of_simulator_object_state(765);
    // EXPECT_TRUE(maybe_value);
    // EXPECT_EQ(maybe_value.value(), "99.99");
}

TEST_F(DcsBiosProtocolTestFixture, get_value_of_simulator_object_state_if_empty)
{
    const unsigned char mock_dcs_bios_message[] = {0x55, 0x55, 0x55, 0x55};
    // mock_dcs.send_string(mock_dcs_message);
    simulator_interface.update_simulator_state();

    auto maybe_value = simulator_interface.get_value_of_simulator_object_state(765);
    EXPECT_FALSE(maybe_value);
}

TEST_F(DcsBiosProtocolTestFixture, get_decimal_of_simulator_object_state_if_nonexistant)
{
    EXPECT_FALSE(simulator_interface.get_decimal_of_simulator_object_state(999));
}

TEST_F(DcsBiosProtocolTestFixture, get_decimal_of_simulator_object_state_with_valid_value)
{
    std::string mock_dcs_message = "header*765=99.99";
    mock_dcs.send_string(mock_dcs_message);
    simulator_interface.update_simulator_state();

    // auto maybe_value = simulator_interface.get_decimal_of_simulator_object_state(765);
    // EXPECT_TRUE(maybe_value);
    // EXPECT_EQ(maybe_value.value(), Decimal("99.99"));
}

TEST_F(DcsBiosProtocolTestFixture, get_decimal_of_simulator_object_state_with_non_decimal_value)
{
    std::string mock_dcs_message = "header*765=STRING";
    mock_dcs.send_string(mock_dcs_message);
    simulator_interface.update_simulator_state();

    auto maybe_value = simulator_interface.get_decimal_of_simulator_object_state(765);
    EXPECT_FALSE(maybe_value);
}

TEST_F(DcsBiosProtocolTestFixture, get_decimal_of_simulator_object_state_if_empty)
{
    std::string mock_dcs_message = "header*765=";
    mock_dcs.send_string(mock_dcs_message);
    simulator_interface.update_simulator_state();

    auto maybe_value = simulator_interface.get_value_of_simulator_object_state(765);
    EXPECT_FALSE(maybe_value);
}

TEST_F(DcsBiosProtocolTestFixture, clear_game_state)
{
    // Send a single message from mock DCS that contains updates for multiple IDs.
    const unsigned char mock_dcs_bios_message_one[] = {0x55, 0x55, 0x55, 0x55,                         //
                                                       0x08, 0x00, 0x02, 0x00, 0x6F, 0x72,             //
                                                       0x02, 0x04, 0x04, 0x00, 0x31, 0x30, 0x2E, 0x30, //
                                                       0x0C, 0x74, 0x02, 0x00, 0x00, 0x02};
    // mock_dcs.send_string(mock_dcs_message_one);
    simulator_interface.update_simulator_state();

    std::unordered_map<int, std::string> current_game_state = simulator_interface.debug_get_current_game_state();
    // EXPECT_TRUE(current_game_state.size() > 0);

    // Test that game state is able to be cleared.
    simulator_interface.clear_game_state();
    current_game_state = simulator_interface.debug_get_current_game_state();
    EXPECT_EQ(0, current_game_state.size());
}

TEST_F(DcsBiosProtocolTestFixture, debug_print_format)
{
    // Send a single message from mock DCS that contains updates for multiple IDs.
    const unsigned char mock_dcs_bios_message_one[] = {0x55, 0x55, 0x55, 0x55,                         //
                                                       0x08, 0x00, 0x02, 0x00, 0x6F, 0x72,             //
                                                       0x02, 0x04, 0x04, 0x00, 0x31, 0x30, 0x2E, 0x30, //
                                                       0x0C, 0x74, 0x02, 0x00, 0x00, 0x02};
    // mock_dcs.send_string(mock_dcs_message_one);
    simulator_interface.update_simulator_state();
    std::unordered_map<int, std::string> current_game_state = simulator_interface.debug_get_current_game_state();

    // EXPECT_EQ(current_game_state[761], "1");
    // EXPECT_EQ(current_game_state[765], "2.00");
    // EXPECT_EQ(current_game_state[2026], "TEXT_STR");
    // EXPECT_EQ(current_game_state[2027], "4");
}
} // namespace test
