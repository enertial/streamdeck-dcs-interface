// Copyright 2020 Charles Tytler

#include "gtest/gtest.h"

#include "../DcsInterface/DcsInterface.cpp"

namespace test
{

    TEST(DcsInterfaceTest, invalid_connection_port_settings)
    {
        DcsConnectionSettings connection_settings = {"19ab", "abc", "127.0.0.1"};
        EXPECT_THROW(DcsInterface dcs_interface(connection_settings), std::runtime_error);
    }

    TEST(DcsInterfaceTest, dcs_reset_command_on_construction)
    {
        DcsConnectionSettings connection_settings = {"1908", "1909", "127.0.0.1"};
        UdpSocket mock_dcs(connection_settings.ip_address, connection_settings.tx_port, connection_settings.rx_port);
        DcsInterface dcs_interface(connection_settings);

        // Test that the reset message "R" is received by DCS on creation of DcsInterface.
        std::stringstream ss = mock_dcs.receive();
        EXPECT_EQ("R", ss.str());
    }

    class DcsInterfaceTestFixture : public ::testing::Test
    {
    public:
        DcsInterfaceTestFixture()
            : // Mock DCS socket uses the reverse rx and tx ports of dcs_interface so it can communicate with it.
              mock_dcs(connection_settings.ip_address, connection_settings.tx_port, connection_settings.rx_port),
              dcs_interface(connection_settings)
        {

            // Consume intial reset command sent to to mock_dcs.
            (void)mock_dcs.receive();
        }

        DcsConnectionSettings connection_settings = {"1908", "1909", "127.0.0.1"};
        UdpSocket mock_dcs;         // A socket that will mock Send/Receive messages from DCS.
        DcsInterface dcs_interface; // DCS Interface to test.
    };

    TEST_F(DcsInterfaceTestFixture, empty_game_state_on_initialization)
    {
        // Test that current game state initializes as empty.
        std::map<int, std::string> current_game_state = dcs_interface.debug_get_current_game_state();
        EXPECT_EQ(0, current_game_state.size());
    }

    TEST_F(DcsInterfaceTestFixture, update_dcs_state)
    {
        // TEST 1 - Received values are stored and retrievable.
        // Send a single message from mock DCS that contains updates for multiple IDs.
        std::string mock_dcs_message = "header*761=1:765=2.00:2026=TEXT_STR:2027=4";
        mock_dcs.send(mock_dcs_message);
        dcs_interface.update_dcs_state();

        // Query for ID updates.
        EXPECT_EQ("1", dcs_interface.get_value_of_dcs_id(761));
        EXPECT_EQ("2.00", dcs_interface.get_value_of_dcs_id(765));
        EXPECT_EQ("TEXT_STR", dcs_interface.get_value_of_dcs_id(2026));
        EXPECT_EQ("4", dcs_interface.get_value_of_dcs_id(2027));

        // TEST 2 - Received values will overwrite their previous values.
        // Send a new message with one ID value updated.
        mock_dcs_message = "header*765=99.99";
        mock_dcs.send(mock_dcs_message);
        dcs_interface.update_dcs_state();

        // Query for ID updates - Test that only the value for ID 765 has changed.
        EXPECT_EQ("99.99", dcs_interface.get_value_of_dcs_id(765));

        EXPECT_EQ("1", dcs_interface.get_value_of_dcs_id(761));
        EXPECT_EQ("TEXT_STR", dcs_interface.get_value_of_dcs_id(2026));
        EXPECT_EQ("4", dcs_interface.get_value_of_dcs_id(2027));
    }

    TEST_F(DcsInterfaceTestFixture, update_dcs_state_handle_newline_chars)
    {
        // Send a single message from mock DCS that contains newline characters at the end of tokens.
        std::string mock_dcs_message = "header*761=1\n:765=2.00\n:2026=TEXT_STR\n:2027=4\n";
        mock_dcs.send(mock_dcs_message);
        dcs_interface.update_dcs_state();

        // Query for ID updates.
        EXPECT_EQ("1", dcs_interface.get_value_of_dcs_id(761));
        EXPECT_EQ("2.00", dcs_interface.get_value_of_dcs_id(765));
        EXPECT_EQ("TEXT_STR", dcs_interface.get_value_of_dcs_id(2026));
        EXPECT_EQ("4", dcs_interface.get_value_of_dcs_id(2027));
    }

    TEST_F(DcsInterfaceTestFixture, update_dcs_state_end_of_mission)
    {
        // Send a single message from mock DCS that contains updates for multiple IDs.
        // Expect that the number of stored values in game state is non-zero.
        std::string mock_dcs_message = "header*761=1:765=2.00:2026=TEXT_STR:2027=4";
        mock_dcs.send(mock_dcs_message);
        dcs_interface.update_dcs_state();
        std::map<int, std::string> current_game_state = dcs_interface.debug_get_current_game_state();
        EXPECT_TRUE(current_game_state.size() > 0);

        // Test that game state is cleared when a "STOP" message is received, signifying end of mission.
        mock_dcs_message = "header*Ikarus=stop";
        mock_dcs.send(mock_dcs_message);
        dcs_interface.update_dcs_state();
        current_game_state = dcs_interface.debug_get_current_game_state();
        EXPECT_EQ(0, current_game_state.size());
    }

    TEST_F(DcsInterfaceTestFixture, send_dcs_command)
    {
        const int button_id = 3250;
        const std::string device_id = "24";
        const std::string value = "1";
        // Message is sent prepended with "C".
        const std::string expected_msg_buffer = "C24,3250,1";

        dcs_interface.send_dcs_command(button_id, device_id, value);
        std::stringstream ss_received = mock_dcs.receive();
        EXPECT_EQ(ss_received.str(), expected_msg_buffer);
    }

    TEST_F(DcsInterfaceTestFixture, send_dcs_reset_command)
    {
        dcs_interface.send_dcs_reset_command();
        std::stringstream ss_received = mock_dcs.receive();
        EXPECT_EQ(ss_received.str(), "R");
    }

    TEST_F(DcsInterfaceTestFixture, get_current_dcs_module_init)
    {
        // Test that the default value of an empty string is returned after initialization.
        EXPECT_EQ("", dcs_interface.get_current_dcs_module());
    }

    TEST_F(DcsInterfaceTestFixture, get_current_dcs_module)
    {
        // Test that game module is updated when the "File" message with module name is received.
        std::string mock_dcs_message = "header*File=AV8BNA";
        mock_dcs.send(mock_dcs_message);
        dcs_interface.update_dcs_state();
        EXPECT_EQ("AV8BNA", dcs_interface.get_current_dcs_module());

        // Test that game module is cleared when a "STOP" message is received, signifying end of mission.
        mock_dcs_message = "header*Ikarus=stop";
        mock_dcs.send(mock_dcs_message);
        dcs_interface.update_dcs_state();
        EXPECT_EQ("", dcs_interface.get_current_dcs_module());
    }

    TEST_F(DcsInterfaceTestFixture, get_value_of_dcs_id_if_nonexistant)
    {
        // Test that the default value of an empty string is returned for a DCS ID with no stored value.
        EXPECT_EQ("", dcs_interface.get_value_of_dcs_id(999));
    }

    TEST_F(DcsInterfaceTestFixture, clear_game_state)
    {
        // Send a single message from mock DCS that contains updates for multiple IDs.
        // Expect that the number of stored values in game state is non-zero.
        std::string mock_dcs_message = "header*761=1:765=2.00:2026=TEXT_STR:2027=4";
        mock_dcs.send(mock_dcs_message);
        dcs_interface.update_dcs_state();
        std::map<int, std::string> current_game_state = dcs_interface.debug_get_current_game_state();
        EXPECT_TRUE(current_game_state.size() > 0);

        // Test that game state is able to be cleared.
        dcs_interface.clear_game_state();
        current_game_state = dcs_interface.debug_get_current_game_state();
        EXPECT_EQ(0, current_game_state.size());
    }

    TEST_F(DcsInterfaceTestFixture, debug_print_format)
    {
        // Send a single message from mock DCS that contains updates for multiple IDs.
        // Expect that the number of stored values in game state is non-zero.
        std::string mock_dcs_message = "header*761=1:765=2.00:2026=TEXT_STR:2027=4";
        mock_dcs.send(mock_dcs_message);
        dcs_interface.update_dcs_state();
        std::map<int, std::string> current_game_state = dcs_interface.debug_get_current_game_state();

        EXPECT_EQ(current_game_state[761], "1");
        EXPECT_EQ(current_game_state[765], "2.00");
        EXPECT_EQ(current_game_state[2026], "TEXT_STR");
        EXPECT_EQ(current_game_state[2027], "4");
    }
} // namespace test
