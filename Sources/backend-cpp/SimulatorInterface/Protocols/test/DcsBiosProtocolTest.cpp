// Copyright 2021 Charles Tytler

#include "gtest/gtest.h"

#include "SimulatorInterface/Protocols/DcsBiosProtocol.h"

namespace test
{
#define SIZE_OF(x) (sizeof(x) / sizeof((x)[0]))

TEST(DcsBiosProtocolTest, invalid_connection_port_settings)
{
    SimulatorConnectionSettings connection_settings = {"19ab", "abc", "127.0.0.1", ""};
    EXPECT_THROW(DcsBiosProtocol simulator_interface(connection_settings), std::runtime_error);
}

TEST(DcsBiosProtocolTest, dcs_reset_command_on_construction)
{
    SimulatorConnectionSettings connection_settings = {"1908", "1909", "127.0.0.1", "239.255.50.11"};
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

TEST_F(DcsBiosProtocolTestFixture, get_current_module_init)
{
    // Test that the default value of an empty string is returned after initialization.
    EXPECT_EQ("", simulator_interface.get_current_module());
}

TEST_F(DcsBiosProtocolTestFixture, get_current_module)
{
    // Test that game module is updated when the ACFT_NAME string is received.
    // clang-format off
    const char mock_dcs_start_message[] = {0x55, 0x55, 0x55, 0x55,                          // Sync frame
                                           0x00, 0x00, 0x08, 0x00,                          // Addr 0x0000 (8 bytes)
                                           'A', 'V', '8', 'B', 'N', 'A', '\0', 0x20,        // String contents
                                           (char)0xFE, (char)0xFF, 0x02, 0x00, 0x00, 0x00}; // End of frame
    // clang-format on
    mock_dcs.send_bytes(mock_dcs_start_message, SIZE_OF(mock_dcs_start_message));
    simulator_interface.update_simulator_state();

    EXPECT_EQ("AV8BNA", simulator_interface.get_current_module());

    // Expect that successive sends with partial strings of Aircraft Name don't interfere with module name.
    // clang-format off
    const char mock_dcs_message[] = {0x55, 0x55, 0x55, 0x55,                          // Sync frame
                                     0x00, 0x00, 0x02, 0x00, 'A', 'V',                // First letters only
                                     (char)0xFE, (char)0xFF, 0x02, 0x00, 0x00, 0x00}; // End of frame
    // clang-format on
    mock_dcs.send_bytes(mock_dcs_message, SIZE_OF(mock_dcs_message));
    simulator_interface.update_simulator_state();

    EXPECT_EQ("AV8BNA", simulator_interface.get_current_module());
}

TEST_F(DcsBiosProtocolTestFixture, update_simulator_state_empty)
{
    // Test that nothing is returned when no state has been received yet.
    simulator_interface.update_simulator_state();
    EXPECT_FALSE(simulator_interface.get_string_at_addr(SimulatorAddress(0x1234, 8)));
    EXPECT_FALSE(simulator_interface.get_value_at_addr(SimulatorAddress(0x5678, 0xFFFF, 0)));
}

TEST_F(DcsBiosProtocolTestFixture, get_integer_value)
{
    // clang-format off
    const char mock_dcs_start_message[] = {0x55, 0x55, 0x55, 0x55,                          // Sync frame
                                           0x78, 0x56, 0x02, 0x00, 0x07, 0x00,              // Addr 0x5678 (2 bytes)
                                           (char)0xFE, (char)0xFF, 0x02, 0x00, 0x00, 0x00}; // End of frame
    // clang-format on
    mock_dcs.send_bytes(mock_dcs_start_message, SIZE_OF(mock_dcs_start_message));
    simulator_interface.update_simulator_state();

    EXPECT_TRUE(Decimal(7) == simulator_interface.get_value_at_addr(SimulatorAddress(0x5678, 0xFFFF, 0)));
}

TEST_F(DcsBiosProtocolTestFixture, get_string_value_max_length)
{
    constexpr int MAX_STRING_LEN = 8;
    // clang-format off
    const char mock_dcs_start_message[] = {0x55, 0x55, 0x55, 0x55,                          // Sync frame
                                           0x34, 0x12, MAX_STRING_LEN, 0x00,                // Addr 0x1234 (8 bytes)
                                           'T', 'e', 's', 't', 'S', 't', 'r', '!',          // String contents
                                           (char)0xFE, (char)0xFF, 0x02, 0x00, 0x00, 0x00}; // End of frame
    // clang-format on
    mock_dcs.send_bytes(mock_dcs_start_message, SIZE_OF(mock_dcs_start_message));
    simulator_interface.update_simulator_state();

    EXPECT_EQ("TestStr!", simulator_interface.get_string_at_addr(SimulatorAddress(0x1234, MAX_STRING_LEN)));
}

TEST_F(DcsBiosProtocolTestFixture, get_string_value_null_terminated)
{
    // clang-format off
    const char mock_dcs_start_message[] = {0x55, 0x55, 0x55, 0x55,                          // Sync frame
                                           0x34, 0x12, 0x08, 0x00,                          // Addr 0x1234 (8 bytes)
                                           'T', 'e', 's', 't', 'S', 't', 'r', '\0',         // String contents
                                           (char)0xFE, (char)0xFF, 0x02, 0x00, 0x00, 0x00}; // End of frame
    // clang-format on
    mock_dcs.send_bytes(mock_dcs_start_message, SIZE_OF(mock_dcs_start_message));
    simulator_interface.update_simulator_state();

    EXPECT_EQ("TestStr", simulator_interface.get_string_at_addr(SimulatorAddress(0x1234, 8)));
}

TEST_F(DcsBiosProtocolTestFixture, update_simulator_state_overwrites_values)
{
    // TEST 1 - Received values are stored and retrievable.
    // Send a single message from mock DCS that contains updates for multiple IDs.
    const char mock_dcs_message_one[] = {0x55,       0x55,       0x55, 0x55,             // Sync frame
                                         0x08,       0x00,       0x02, 0x00, 0x6F, 0x72, // Addr 0x0008 (2 bytes)
                                         0x02,       0x04,       0x04, 0x00, 0x31, 0x30,
                                         0x2E,       0x30,                                // Addr 0x0402 (4 bytes)
                                         0x0C,       0x74,       0x02, 0x00, 0x00, 0x02,  // Addr 0x740C (2 bytes)
                                         (char)0xFE, (char)0xFF, 0x02, 0x00, 0x00, 0x00}; // End of frame
    mock_dcs.send_bytes(mock_dcs_message_one, SIZE_OF(mock_dcs_message_one));
    simulator_interface.update_simulator_state();

    // Query for ID updates.
    const auto address_0x740C = SimulatorAddress{0x740C, 0xFFFF, 0};
    auto data_at_0x740C = simulator_interface.get_value_at_addr(address_0x740C);
    EXPECT_TRUE(data_at_0x740C.value() == Decimal(0x0200));

    // TEST 2 - Received values will overwrite their previous values.
    // Send a new message with one ID value updated.
    const char mock_dcs_message_two[] = {0x55, 0x55, 0x55, 0x55, 0x0C, 0x74, 0x02, 0x00, 0x22, 0x00};
    mock_dcs.send_bytes(mock_dcs_message_two, SIZE_OF(mock_dcs_message_two));
    simulator_interface.update_simulator_state();

    // Query for ID updates - Test that only the value for address x740C has changed.
    data_at_0x740C = simulator_interface.get_value_at_addr(address_0x740C);
    EXPECT_TRUE(data_at_0x740C.value() == Decimal(0x0022));
}

TEST_F(DcsBiosProtocolTestFixture, update_simulator_state_handle_incomplete_message)
{
    // clang-format off
    const char mock_dcs_message[] = {0x55, 0x55, 0x55, 0x55,                         // Sync frame
                                     0x08, 0x00, 0x02, 0x00, 0x6F, 0x72,             //
                                     0x02, 0x04, 0x04};                              // Cut-off frame
    // clang-format on
    mock_dcs.send_bytes(mock_dcs_message, SIZE_OF(mock_dcs_message));
    simulator_interface.update_simulator_state();

    // Query for ID updates.
    const auto data_at_0x0008 = simulator_interface.get_value_at_addr(SimulatorAddress(0x0008, 0xFFFF, 0));
    EXPECT_TRUE(data_at_0x0008.value() == Decimal(0x726F));
}

TEST_F(DcsBiosProtocolTestFixture, update_simulator_state_handle_change_of_module)
{
    // Send a message with module name "ACFT".
    const char mock_dcs_message_one[] = {0x55,       0x55,       0x55, 0x55,              //
                                         0x00,       0x00,       0x06, 0x00,              //
                                         'A',        'C',        'F',  'T',  '\0', 0x20,  // ACFT_NAME = "ACFT"
                                         0x10,       0x11,       0x02, 0x00, 0x01, 0x23,  // Data at Address 0x1110
                                         0x12,       0x11,       0x02, 0x00, 0x02, 0x23,  // Data at Address 0x1112
                                         (char)0xFE, (char)0xFF, 0x02, 0x00, 0x00, 0x00}; // End of frame
    mock_dcs.send_bytes(mock_dcs_message_one, SIZE_OF(mock_dcs_message_one));
    simulator_interface.update_simulator_state();
    // Verify
    EXPECT_EQ("ACFT", simulator_interface.get_current_module());
    const auto address_0x1110 = SimulatorAddress{0x1110, 0xFFFF, 0};
    const auto address_0x1112 = SimulatorAddress{0x1112, 0xFFFF, 0};
    EXPECT_TRUE(simulator_interface.get_value_at_addr(address_0x1110));
    EXPECT_TRUE(simulator_interface.get_value_at_addr(address_0x1112));

    // Send a message with a new module, named "MOD".
    const char mock_dcs_message_two[] = {0x55,       0x55,       0x55, 0x55,              //
                                         0x00,       0x00,       0x06, 0x00,              //
                                         'M',        'O',        'D',  '\0', 0x20, 0x20,  // ACFT_NAME = "ACFT"
                                         0x10,       0x21,       0x02, 0x00, 0x00, 0x22,  // Data at Address 0x2110
                                         (char)0xFE, (char)0xFF, 0x02, 0x00, 0x00, 0x00}; // End of frame
    mock_dcs.send_bytes(mock_dcs_message_two, SIZE_OF(mock_dcs_message_two));
    simulator_interface.update_simulator_state();
    // Verify module name changes, and only most recent data frame data is kept.
    EXPECT_EQ("MOD", simulator_interface.get_current_module());
    EXPECT_FALSE(simulator_interface.get_value_at_addr(address_0x1110));
    EXPECT_FALSE(simulator_interface.get_value_at_addr(address_0x1112));
    const auto address_0x2110 = SimulatorAddress{0x2110, 0xFFFF, 0};
    EXPECT_TRUE(simulator_interface.get_value_at_addr(address_0x2110));

    // Send a message with ACFT_NAME="" (starts with a '\0' character) and No data.
    const char mock_dcs_end_message[] = {0x55,       0x55,       0x55, 0x55, //
                                         0x00,       0x00,       0x06, 0x00, 0x00, 0x20,
                                         0x20,       0x20,       0x20, 0x20,              // Empty ACFT_NAME
                                         (char)0xFE, (char)0xFF, 0x02, 0x00, 0x00, 0x00}; // End of frame
    mock_dcs.send_bytes(mock_dcs_end_message, SIZE_OF(mock_dcs_end_message));
    simulator_interface.update_simulator_state();
    // Verify
    EXPECT_EQ("", simulator_interface.get_current_module());
    // Test that game state is cleared.
    EXPECT_FALSE(simulator_interface.get_value_at_addr(address_0x1110));
}

TEST_F(DcsBiosProtocolTestFixture, send_command)
{
    const std::string control_reference = "BIOS_HANDLE";
    const std::string value = "1";
    const std::string expected_msg_buffer = "BIOS_HANDLE 1\n";

    simulator_interface.send_command(control_reference, value);
    std::stringstream ss_received = mock_dcs.receive_stream();
    EXPECT_EQ(ss_received.str(), expected_msg_buffer);
}

TEST_F(DcsBiosProtocolTestFixture, send_reset_command)
{
    simulator_interface.send_reset_command();
    std::stringstream ss_received = mock_dcs.receive_stream();
    EXPECT_EQ(ss_received.str(), "SYNC E\n");
}

TEST_F(DcsBiosProtocolTestFixture, get_string_at_addr_if_nonexistant)
{
    EXPECT_FALSE(simulator_interface.get_string_at_addr(SimulatorAddress(0x1234)));
    EXPECT_FALSE(simulator_interface.get_string_at_addr(SimulatorAddress(0x1234, 0xFFFF, 0)));
    EXPECT_FALSE(simulator_interface.get_string_at_addr(SimulatorAddress(0x1234, 10)));
}

TEST_F(DcsBiosProtocolTestFixture, clear_game_state)
{
    // Send a single message from mock DCS that contains updates for multiple IDs.
    const char mock_dcs_message[] = {0x55, 0x55, 0x55, 0x55,                         //
                                     0x08, 0x00, 0x02, 0x00, 0x6F, 0x72,             //
                                     0x02, 0x04, 0x04, 0x00, 0x31, 0x30, 0x2E, 0x30, //
                                     0x0C, 0x74, 0x02, 0x00, 0x00, 0x02};
    mock_dcs.send_bytes(mock_dcs_message, SIZE_OF(mock_dcs_message));
    simulator_interface.update_simulator_state();

    auto current_game_state = simulator_interface.get_current_state_as_json();
    EXPECT_EQ(current_game_state.size(), 4);

    // Test that game state is able to be cleared.
    simulator_interface.clear_game_state();
    current_game_state = simulator_interface.get_current_state_as_json();
    EXPECT_EQ(0, current_game_state.size());
}

TEST_F(DcsBiosProtocolTestFixture, debug_print_format)
{
    // Send a single message from mock DCS that contains updates for multiple IDs.
    const char mock_dcs_message[] = {0x55, 0x55, 0x55, 0x55,                         //
                                     0x08, 0x00, 0x02, 0x00, 0x6F, 0x72,             //
                                     0x02, 0x04, 0x04, 0x00, 0x31, 0x30, 0x2E, 0x30, //
                                     0x0C, 0x74, 0x02, 0x00, 0x00, 0x02};
    mock_dcs.send_bytes(mock_dcs_message, SIZE_OF(mock_dcs_message));
    simulator_interface.update_simulator_state();
    const auto current_game_state = simulator_interface.get_current_state_as_json();

    EXPECT_EQ(current_game_state["8"], 0x726F);
    EXPECT_EQ(current_game_state["1026"], 0x3031);
    EXPECT_EQ(current_game_state["1028"], 0x302E);
    EXPECT_EQ(current_game_state["29708"], 0x0200);
}
} // namespace test
