// Copyright 2020 Charles Tytler

#include "gtest/gtest.h"
#include "../Windows/pch.h"

// Create mock version of ESDConnectionManager for testing.
#define UNIT_TEST
const int kESDSDKTarget_HardwareAndSoftware = 0;

class ESDConnectionManager
{
public:
    void SetState(int state, std::string context)
    {
        context_ = context;
        state_ = state;
    }
    void SetTitle(std::string title, std::string context, int flag)
    {
        context_ = context;
        title_ = title;
    }

    std::string context_ = "";
    int state_ = 0;
    std::string title_ = "";
};

#include "../StreamdeckContext.h"
#include "../StreamdeckContext.cpp"

// Constants to be used for providing a valid DcsInterface.
const std::string kDcsListenerPort = "1908";   // Port number to receive DCS updates from.
const std::string kDcsSendPort = "1909";       // Port number which DCS commands will be sent to.
const std::string kDcsIpAddress = "127.0.0.1"; // IP Address on which to communicate with DCS -- Default LocalHost.

TEST(StreamdeckContextTest, update_context_state)
{
    // Open the interface to test and a socket that will mock Send/Receive messages from DCS.
    DcsInterface dcs_interface(kDcsListenerPort, kDcsSendPort, kDcsIpAddress);
    DcsSocket mock_dcs(kDcsSendPort, kDcsListenerPort, kDcsIpAddress);
    ESDConnectionManager esd_connection_manager;
    const std::string context_a = "abc123";
    const json settings = {{"dcs_id_compare_monitor", "765"},
                           {"dcs_id_compare_condition", "EQUAL_TO"},
                           {"dcs_id_comparison_value", "2.0"},
                           {"dcs_id_string_monitor", "2026"}};
    StreamdeckContext streamdeck_context_a(context_a, settings);

    // Test 1 -- With an unpopulated game state in dcs_interface, try to update context state.
    streamdeck_context_a.updateContextState(dcs_interface, &esd_connection_manager);
    // Expect context to be empty since no SetState or SetTitle functions should be called.
    EXPECT_EQ(esd_connection_manager.context_, "");
    EXPECT_EQ(esd_connection_manager.state_, 0);
    EXPECT_EQ(esd_connection_manager.title_, "");

    // Test 2 -- With a populated game state in dcs_interface, try to update context state.
    // Send a single message from mock DCS that contains updates for multiple IDs.
    std::string mock_dcs_message = "header*761=1:765=2.00:2026=TEXT_STR:2027=4";
    mock_dcs.DcsSend(mock_dcs_message);
    dcs_interface.update_dcs_state();
    streamdeck_context_a.updateContextState(dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, context_a);
    EXPECT_EQ(esd_connection_manager.state_, 1);
    EXPECT_EQ(esd_connection_manager.title_, "TEXT_STR");
}