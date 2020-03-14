// Copyright 2020 Charles Tytler

#include "gtest/gtest.h"

#include "../DcsInterface.h"
#include "../DcsInterface.cpp" // Included because I can't figure out Windows Linker

namespace test
{

const std::string kDcsListenerPort = "1908";   // Port number to receive DCS updates from.
const std::string kDcsSendPort = "1909";       // Port number which DCS commands will be sent to.
const std::string kDcsIpAddress = "127.0.0.1"; // IP Address on which to communicate with DCS -- Default LocalHost.

TEST(DcsInterfaceTest, empty_game_state_on_initialization)
{
    // Open the interface to test and a socket that will mock Send/Receive messages from DCS.
    DcsInterface dcs_interface(kDcsListenerPort, kDcsSendPort, kDcsIpAddress);
    DcsSocket mock_dcs(kDcsSendPort, kDcsListenerPort, kDcsIpAddress);

    std::vector<std::string> current_game_state = {};

    // Test that current game state initializes as empty.
    current_game_state = dcs_interface.debug_get_current_game_state();
    EXPECT_EQ(0, current_game_state.size());
}

TEST(DcsInterfaceTest, update_dcs_state)
{
    // Open the interface to test and a socket that will mock Send/Receive messages from DCS.
    DcsInterface dcs_interface(kDcsListenerPort, kDcsSendPort, kDcsIpAddress);
    DcsSocket mock_dcs(kDcsSendPort, kDcsListenerPort, kDcsIpAddress);

    // TEST 1 - Received values are stored and retrievable.
    // Send a single message from mock DCS that contains updates for multiple IDs.
    std::string mock_dcs_message = "header*761=1:765=2.00:2026=TEXT_STR:2027=4";
    mock_dcs.DcsSend(mock_dcs_message);
    dcs_interface.update_dcs_state();

    // Query for ID updates.
    EXPECT_EQ("1", dcs_interface.get_value_of_dcs_id(761));
    EXPECT_EQ("2.00", dcs_interface.get_value_of_dcs_id(765));
    EXPECT_EQ("TEXT_STR", dcs_interface.get_value_of_dcs_id(2026));
    EXPECT_EQ("4", dcs_interface.get_value_of_dcs_id(2027));

    // TEST 2 - Received values will overwrite their previous values.
    // Send a new message with one ID value updated.
    mock_dcs_message = "header*765=99.99";
    mock_dcs.DcsSend(mock_dcs_message);
    dcs_interface.update_dcs_state();

    // Query for ID updates - Test that only the value for ID 765 has changed.
    EXPECT_EQ("99.99", dcs_interface.get_value_of_dcs_id(765));

    EXPECT_EQ("1", dcs_interface.get_value_of_dcs_id(761));
    EXPECT_EQ("TEXT_STR", dcs_interface.get_value_of_dcs_id(2026));
    EXPECT_EQ("4", dcs_interface.get_value_of_dcs_id(2027));
}

TEST(DcsInterfaceTest, update_dcs_state_end_of_mission)
{
    // Open the interface to test and a socket that will mock Send/Receive messages from DCS.
    DcsInterface dcs_interface(kDcsListenerPort, kDcsSendPort, kDcsIpAddress);
    DcsSocket mock_dcs(kDcsSendPort, kDcsListenerPort, kDcsIpAddress);

    // Send a single message from mock DCS that contains updates for multiple IDs.
    // Expect that the number of stored values in game state is non-zero.
    std::string mock_dcs_message = "header*761=1:765=2.00:2026=TEXT_STR:2027=4";
    mock_dcs.DcsSend(mock_dcs_message);
    dcs_interface.update_dcs_state();
    std::vector<std::string> current_game_state = dcs_interface.debug_get_current_game_state();
    EXPECT_TRUE(current_game_state.size() > 0);

    // Test that game state is cleared when a "STOP" message is received, signifying end of mission.
    mock_dcs_message = "header*Ikarus=stop";
    mock_dcs.DcsSend(mock_dcs_message);
    dcs_interface.update_dcs_state();
    current_game_state = dcs_interface.debug_get_current_game_state();
    EXPECT_EQ(0, current_game_state.size());
}

TEST(DcsInterfaceTest, send_dcs_command)
{
    // Open the interface to test and a socket that will mock Send/Receive messages from DCS.
    DcsInterface dcs_interface(kDcsListenerPort, kDcsSendPort, kDcsIpAddress);
    DcsSocket mock_dcs(kDcsSendPort, kDcsListenerPort, kDcsIpAddress);

    const int button_id = 250;
    const std::string device_id = "24";
    const std::string value = "1";
    // Message is sent prepended with "C" prepended and 3000 added to Button ID.
    const std::string expected_msg_buffer = "C24,3250,1";

    dcs_interface.send_dcs_command(button_id, device_id, value);
    std::stringstream ss_received = mock_dcs.DcsReceive();
    EXPECT_EQ(ss_received.str(), expected_msg_buffer);
}

TEST(DcsInterfaceTest, get_current_dcs_module)
{
    // Open the interface to test and a socket that will mock Send/Receive messages from DCS.
    DcsInterface dcs_interface(kDcsListenerPort, kDcsSendPort, kDcsIpAddress);
    DcsSocket mock_dcs(kDcsSendPort, kDcsListenerPort, kDcsIpAddress);

    // Test that the default value of an empty string is returned after initialization.
    EXPECT_EQ("", dcs_interface.get_current_dcs_module());

    // Test that game module is updated when the "File" message with module name is received.
    std::string mock_dcs_message = "header*File=AV8BNA";
    mock_dcs.DcsSend(mock_dcs_message);
    dcs_interface.update_dcs_state();
    EXPECT_EQ("AV8BNA", dcs_interface.get_current_dcs_module());

    // Test that game module is cleared when a "STOP" message is received, signifying end of mission.
    mock_dcs_message = "header*Ikarus=stop";
    mock_dcs.DcsSend(mock_dcs_message);
    dcs_interface.update_dcs_state();
    EXPECT_EQ("", dcs_interface.get_current_dcs_module());
}

TEST(DcsInterfaceTest, get_value_of_dcs_id_if_nonexistant)
{
    // Open the interface to test and a socket that will mock Send/Receive messages from DCS.
    DcsInterface dcs_interface(kDcsListenerPort, kDcsSendPort, kDcsIpAddress);
    DcsSocket mock_dcs(kDcsSendPort, kDcsListenerPort, kDcsIpAddress);

    // Test that the default value of an empty string is returned for a DCS ID with no stored value.
    EXPECT_EQ("", dcs_interface.get_value_of_dcs_id(999));
}

TEST(DcsInterfaceTest, clear_game_state)
{
    // Open the interface to test and a socket that will mock Send/Receive messages from DCS.
    DcsInterface dcs_interface(kDcsListenerPort, kDcsSendPort, kDcsIpAddress);
    DcsSocket mock_dcs(kDcsSendPort, kDcsListenerPort, kDcsIpAddress);

    // Send a single message from mock DCS that contains updates for multiple IDs.
    // Expect that the number of stored values in game state is non-zero.
    std::string mock_dcs_message = "header*761=1:765=2.00:2026=TEXT_STR:2027=4";
    mock_dcs.DcsSend(mock_dcs_message);
    dcs_interface.update_dcs_state();
    std::vector<std::string> current_game_state = dcs_interface.debug_get_current_game_state();
    EXPECT_TRUE(current_game_state.size() > 0);

    // Test that game state is able to be cleared.
    dcs_interface.clear_game_state();
    current_game_state = dcs_interface.debug_get_current_game_state();
    EXPECT_EQ(0, current_game_state.size());
}

TEST(DcsInterfaceTest, debug_print_format)
{
    // Open the interface to test and a socket that will mock Send/Receive messages from DCS.
    DcsInterface dcs_interface(kDcsListenerPort, kDcsSendPort, kDcsIpAddress);
    DcsSocket mock_dcs(kDcsSendPort, kDcsListenerPort, kDcsIpAddress);

    // Send a single message from mock DCS that contains updates for multiple IDs.
    // Expect that the number of stored values in game state is non-zero.
    std::string mock_dcs_message = "header*761=1:765=2.00:2026=TEXT_STR:2027=4";
    mock_dcs.DcsSend(mock_dcs_message);
    dcs_interface.update_dcs_state();
    std::vector<std::string> current_game_state = dcs_interface.debug_get_current_game_state();

    EXPECT_EQ("761: 1\n", current_game_state[0]);
    EXPECT_EQ("765: 2.00\n", current_game_state[1]);
    EXPECT_EQ("2026: TEXT_STR\n", current_game_state[2]);
    EXPECT_EQ("2027: 4\n", current_game_state[3]);
}
} // namespace test
