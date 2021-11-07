// Copyright 2020 Charles Tytler

#include "gtest/gtest.h"
#include <unordered_map>

#include "Test/MockESDConnectionManager.h"

#include "StreamdeckContext/StreamdeckContext.h"

#include "SimulatorInterface/Backends/DcsExportScriptInterface.h"
#include "StreamdeckContext/SendActions/IncrementContext.h"
#include "StreamdeckContext/SendActions/MomentaryContext.h"
#include "StreamdeckContext/SendActions/SwitchContext.h"

TEST(StreamdeckContextTest, update_context_state_when_no_dcs)
{
    // Test -- With an unpopulated game state in simulator_interface, try to update context state.
    StreamdeckContext test_context("def456");
    SimulatorConnectionSettings connection_settings = {"2304", "2305", "127.0.0.1"};
    DcsExportScriptInterface simulator_interface(connection_settings);
    MockESDConnectionManager esd_connection_manager{};
    test_context.updateContextState(simulator_interface, &esd_connection_manager);
    // Expect no state or title change as default context state and title values have not changed.
    EXPECT_EQ(esd_connection_manager.context_, "");
    EXPECT_EQ(esd_connection_manager.num_calls_to_SetState, 0);
}

class StreamdeckContextTestFixture : public ::testing::Test
{
  public:
    StreamdeckContextTestFixture()
        : // Mock DCS socket uses the reverse rx and tx ports of simulator_interface so it can communicate with it.
          mock_dcs(connection_settings.ip_address, connection_settings.tx_port, connection_settings.rx_port),
          simulator_interface(connection_settings), fixture_context(fixture_context_id)
    {
        // Consume intial reset command sent to to mock_dcs.
        (void)mock_dcs.receive();

        // Send a single message from mock DCS that contains update for monitored ID.
        std::string mock_dcs_message = "header*761=1:765=2.00:2026=TEXT_STR:2027=0.1";
        mock_dcs.send(mock_dcs_message);
        simulator_interface.update_simulator_state();
    }

    SimulatorConnectionSettings connection_settings = {"1908", "1909", "127.0.0.1"};
    UdpSocket mock_dcs;                                // A socket that will mock Send/Receive messages from DCS.
    DcsExportScriptInterface simulator_interface;      // Simulator Interface to test.
    MockESDConnectionManager esd_connection_manager{}; // Streamdeck connection manager, using mock class definition.

    StreamdeckContext fixture_context;

    // Create StreamdeckContext to test without any defined settings.
    static inline std::string fixture_context_id = "abc123";
};

TEST_F(StreamdeckContextTestFixture, UseImageStateMonitor)
{
    // Create StreamdeckContext initialized with settings to test.
    const std::string context_id = "def456";
    const json settings = {{"dcs_id_compare_monitor", "765"},
                           {"dcs_id_compare_condition", "EQUAL_TO"},
                           {"dcs_id_comparison_value", "2.0"}};
    StreamdeckContext test_context(context_id, settings);
    test_context.updateContextState(simulator_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, context_id);
    EXPECT_EQ(esd_connection_manager.state_, 1);
}

TEST_F(StreamdeckContextTestFixture, UseTitleMonitor)
{
    // Create StreamdeckContext initialized with settings to test.
    const std::string context_id = "def456";
    const json settings = {{"dcs_id_string_monitor", "2026"}, {"string_monitor_passthrough_check", true}};
    StreamdeckContext test_context(context_id, settings);
    test_context.updateContextState(simulator_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, context_id);
    EXPECT_EQ(esd_connection_manager.title_, "TEXT_STR");
}

TEST_F(StreamdeckContextTestFixture, UpdateContextSettings)
{
    // Test 1 -- With no settings defined, streamdeck context should not send update.
    fixture_context.updateContextState(simulator_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, "");
    EXPECT_EQ(esd_connection_manager.num_calls_to_SetState, 0);

    // Test 2 -- With populated settings, streamdeck context should try to update context state.
    json settings = {{"dcs_id_compare_monitor", "765"},
                     {"dcs_id_compare_condition", "EQUAL_TO"},
                     {"dcs_id_comparison_value", "2.0"},
                     {"dcs_id_string_monitor", "2026"},
                     {"string_monitor_passthrough_check", true}};
    fixture_context.updateContextSettings(settings);
    fixture_context.updateContextState(simulator_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, fixture_context_id);
    EXPECT_EQ(esd_connection_manager.state_, 1);
    EXPECT_EQ(esd_connection_manager.title_, "TEXT_STR");

    // Test 3 -- With cleared settings, streamdeck context should send default state and title.
    settings = {{"dcs_id_compare_monitor", ""},
                {"dcs_id_compare_condition", "EQUAL_TO"}, //< selection type always has some value.
                {"dcs_id_comparison_value", ""},
                {"dcs_id_string_monitor", ""}};
    fixture_context.updateContextSettings(settings);
    fixture_context.updateContextState(simulator_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, fixture_context_id);
    EXPECT_EQ(esd_connection_manager.state_, 0);
    EXPECT_EQ(esd_connection_manager.title_, "");
}

TEST_F(StreamdeckContextTestFixture, force_send_state_update)
{
    // Test 1 -- With updateContextState and no detected state changes, no state is sent to connection manager.
    fixture_context.updateContextState(simulator_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, "");
    EXPECT_EQ(esd_connection_manager.num_calls_to_SetState, 0);

    // Test -- force send will send current state regardless of state change.
    fixture_context.forceSendState(&esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, "abc123");
    EXPECT_EQ(esd_connection_manager.state_, 0);
    EXPECT_EQ(esd_connection_manager.num_calls_to_SetState, 1);
}

TEST_F(StreamdeckContextTestFixture, force_send_state_update_with_zero_delay)
{
    // Test 1 -- With updateContextState and no detected state changes, no state is sent to connection manager.
    fixture_context.updateContextState(simulator_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, "");
    EXPECT_EQ(esd_connection_manager.num_calls_to_SetState, 0);

    // Test -- force send will send current state regardless of state change.
    int delay_count = 0;
    fixture_context.forceSendStateAfterDelay(delay_count);
    fixture_context.updateContextState(simulator_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, "abc123");
    EXPECT_EQ(esd_connection_manager.state_, 0);
    EXPECT_EQ(esd_connection_manager.num_calls_to_SetState, 1);
}

TEST_F(StreamdeckContextTestFixture, force_send_state_update_after_delay)
{
    // Test -- force send will send current state regardless of state change.
    int delay_count = 3;
    fixture_context.forceSendStateAfterDelay(delay_count);
    while (delay_count > 0) {
        fixture_context.updateContextState(simulator_interface, &esd_connection_manager);
        EXPECT_EQ(esd_connection_manager.context_, "");
        EXPECT_EQ(esd_connection_manager.num_calls_to_SetState, 0);
        delay_count--;
    }
    fixture_context.updateContextState(simulator_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, "abc123");
    EXPECT_EQ(esd_connection_manager.state_, 0);
    EXPECT_EQ(esd_connection_manager.num_calls_to_SetState, 1);
}

TEST_F(StreamdeckContextTestFixture, force_send_state_update_negative_delay)
{
    // Test 1 -- With updateContextState and no detected state changes, no state is sent to connection manager.
    fixture_context.updateContextState(simulator_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, "");
    EXPECT_EQ(esd_connection_manager.num_calls_to_SetState, 0);

    // Test -- force send will occur as delay is already less than zero.
    int delay_count = -3;
    fixture_context.forceSendStateAfterDelay(delay_count);
    fixture_context.updateContextState(simulator_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, "abc123");
    EXPECT_EQ(esd_connection_manager.state_, 0);
    EXPECT_EQ(esd_connection_manager.num_calls_to_SetState, 1);
}

TEST_F(StreamdeckContextTestFixture, derived_class_instances_within_container)
{
    // This is more of a test of the StreamdeckInterface.cpp use of this class than of the class itself.

    std::string mock_dcs_message = "header*1=a:2=b:3=c";
    mock_dcs.send(mock_dcs_message);
    simulator_interface.update_simulator_state();

    std::unordered_map<std::string, std::unique_ptr<StreamdeckContext>> ctx_map;
    ctx_map["ctx_a"] =
        std::unique_ptr<StreamdeckContext>(new StreamdeckContext("ctx_a", {{"dcs_id_string_monitor", "1"}}));
    ctx_map["ctx_b"] =
        std::unique_ptr<StreamdeckContext>(new IncrementContext("ctx_b", {{"dcs_id_string_monitor", "2"}}));
    ctx_map["ctx_c"] = std::unique_ptr<StreamdeckContext>(new SwitchContext("ctx_c", {{"dcs_id_string_monitor", "3"}}));

    // Update state through map key.
    ctx_map["ctx_b"]->updateContextState(simulator_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, "ctx_b");
    EXPECT_EQ(esd_connection_manager.title_, "b");

    // Update settings through map key.
    ctx_map["ctx_b"]->updateContextSettings({{"dcs_id_string_monitor", "1"}});

    // Test that new settings are reflected in state send.
    ctx_map["ctx_b"]->updateContextState(simulator_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, "ctx_b");
    EXPECT_EQ(esd_connection_manager.title_, "a");
}
