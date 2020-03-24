// Copyright 2020 Charles Tytler

#include "../Windows/pch.h"
#include "gtest/gtest.h"
#include <unordered_map>

// Create mock version of ESDConnectionManager for testing.
#define UNIT_TEST
const int kESDSDKTarget_HardwareAndSoftware = 0;

class ESDConnectionManager {
  public:
    void SetState(int state, std::string context) {
        context_ = context;
        state_ = state;
    }
    void SetTitle(std::string title, std::string context, int flag) {
        context_ = context;
        title_ = title;
    }

    // Only in mock class:
    void clear_buffer() {
        context_ = "";
        state_ = 0;
        title_ = "";
    }

    std::string context_ = "";
    int state_ = 0;
    std::string title_ = "";
};

#include "../StreamdeckContext.cpp"

class StreamdeckContextTestFixture : public ::testing::Test {
  public:
    StreamdeckContextTestFixture()
        : dcs_interface(kDcsListenerPort, kDcsSendPort, kDcsIpAddress),
          mock_dcs(kDcsSendPort, kDcsListenerPort, kDcsIpAddress) {}

    DcsInterface dcs_interface;                  // DCS Interface to test.
    DcsSocket mock_dcs;                          // A socket that will mock Send/Receive messages from DCS.
    ESDConnectionManager esd_connection_manager; // Streamdeck connection manager, using mock class definition.

    // Constants to be used for providing a valid DcsInterface.
    static inline std::string kDcsListenerPort = "1908"; // Port number to receive DCS updates from.
    static inline std::string kDcsSendPort = "1909";     // Port number which DCS commands will be sent to.
    static inline std::string kDcsIpAddress =
        "127.0.0.1"; // IP Address on which to communicate with DCS -- Default LocalHost.
};

TEST_F(StreamdeckContextTestFixture, update_context_state) {
    // Create StreamdeckContext to test.
    const std::string context_a = "abc123";
    const json settings = {{"dcs_id_compare_monitor", "765"},
                           {"dcs_id_compare_condition", "EQUAL_TO"},
                           {"dcs_id_comparison_value", "2.0"},
                           {"dcs_id_string_monitor", "2026"}};
    StreamdeckContext streamdeck_context_a(context_a, settings);

    // Test 1 -- With an unpopulated game state in dcs_interface, try to update context state.
    streamdeck_context_a.updateContextState(dcs_interface, &esd_connection_manager);
    // Expect no state or title change as default context state and title values have not changed.
    EXPECT_EQ(esd_connection_manager.context_, "");

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

TEST_F(StreamdeckContextTestFixture, update_context_settings) {
    // Send a single message from mock DCS that contains updates for multiple IDs.
    std::string mock_dcs_message = "header*761=1:765=2.00:2026=TEXT_STR:2027=4";
    mock_dcs.DcsSend(mock_dcs_message);
    dcs_interface.update_dcs_state();

    // Create StreamdeckContext to test without any defined settings.
    const std::string context_a = "abc123";
    StreamdeckContext streamdeck_context_a(context_a);

    // Test 1 -- With no settings defined, streamdeck context should not send update.
    streamdeck_context_a.updateContextState(dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, "");

    // Test 2 -- With populated settings, streamdeck context should try to update context state.
    json settings = {{"dcs_id_compare_monitor", "765"},
                     {"dcs_id_compare_condition", "EQUAL_TO"},
                     {"dcs_id_comparison_value", "2.0"},
                     {"dcs_id_string_monitor", "2026"}};
    streamdeck_context_a.updateContextSettings(settings);
    streamdeck_context_a.updateContextState(dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, context_a);
    EXPECT_EQ(esd_connection_manager.state_, 1);
    EXPECT_EQ(esd_connection_manager.title_, "TEXT_STR");

    // Test 3 -- With cleared settings, streamdeck context should send default state and title.
    settings = {{"dcs_id_compare_monitor", ""},
                {"dcs_id_compare_condition", "EQUAL_TO"}, //< selection type always has some value.
                {"dcs_id_comparison_value", ""},
                {"dcs_id_string_monitor", ""}};
    streamdeck_context_a.updateContextSettings(settings);
    streamdeck_context_a.updateContextState(dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, context_a);
    EXPECT_EQ(esd_connection_manager.state_, 0);
    EXPECT_EQ(esd_connection_manager.title_, "");
}

TEST_F(StreamdeckContextTestFixture, update_context_state_float_comparisons) {
    const std::string comparison_value_as_str = "1.56";
    const float comparison_value_as_flt = 1.56F;
    json settings = {{"dcs_id_compare_monitor", "123"},
                     {"dcs_id_compare_condition", "EQUAL_TO"},
                     {"dcs_id_comparison_value", comparison_value_as_str}};

    // Create StreamdeckContexts with different comparison selections to test.
    StreamdeckContext context_with_equals("ctx_equals", settings);
    settings["dcs_id_compare_condition"] = "LESS_THAN";
    StreamdeckContext context_with_less_than("ctx_less_than", settings);
    settings["dcs_id_compare_condition"] = "GREATER_THAN";
    StreamdeckContext context_with_greater_than("ctx_greater_than", settings);

    // Test 1 -- Compare 0 to non-zero comparison_value.
    std::string mock_dcs_message = "header*123=0";
    mock_dcs.DcsSend(mock_dcs_message);
    dcs_interface.update_dcs_state();

    esd_connection_manager.clear_buffer();
    context_with_equals.updateContextState(dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, "");
    esd_connection_manager.clear_buffer();
    context_with_less_than.updateContextState(dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, "ctx_less_than");
    EXPECT_EQ(esd_connection_manager.state_, 1);
    esd_connection_manager.clear_buffer();
    context_with_greater_than.updateContextState(dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, "");

    // Test 2 -- Compare value less than comparison_value.
    mock_dcs_message = "header*123=" + std::to_string(comparison_value_as_flt / 2.0F);
    mock_dcs.DcsSend(mock_dcs_message);
    dcs_interface.update_dcs_state();

    esd_connection_manager.clear_buffer();
    context_with_equals.updateContextState(dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, "");
    esd_connection_manager.clear_buffer();
    context_with_less_than.updateContextState(dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, ""); //< Does not send because no change (coniditon still satisfied).
    esd_connection_manager.clear_buffer();
    context_with_greater_than.updateContextState(dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, "");

    // Test 3 -- Compare value equal to comparison_value.
    mock_dcs_message = "header*123=" + std::to_string(comparison_value_as_flt);
    mock_dcs.DcsSend(mock_dcs_message);
    dcs_interface.update_dcs_state();

    esd_connection_manager.clear_buffer();
    context_with_equals.updateContextState(dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, "ctx_equals");
    EXPECT_EQ(esd_connection_manager.state_, 1);
    esd_connection_manager.clear_buffer();
    context_with_less_than.updateContextState(dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, "ctx_less_than"); //< Sends because of state change.
    EXPECT_EQ(esd_connection_manager.state_, 0);
    esd_connection_manager.clear_buffer();
    context_with_greater_than.updateContextState(dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, "");

    // Test 4 -- Compare value greater than comparison_value.
    mock_dcs_message = "header*123=" + std::to_string(comparison_value_as_flt * 2.0F);
    mock_dcs.DcsSend(mock_dcs_message);
    dcs_interface.update_dcs_state();

    esd_connection_manager.clear_buffer();
    context_with_equals.updateContextState(dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, "ctx_equals"); //< Sends because of state change.
    EXPECT_EQ(esd_connection_manager.state_, 0);
    esd_connection_manager.clear_buffer();
    context_with_less_than.updateContextState(dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, "");
    esd_connection_manager.clear_buffer();
    context_with_greater_than.updateContextState(dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, "ctx_greater_than");
    EXPECT_EQ(esd_connection_manager.state_, 1);
}

TEST_F(StreamdeckContextTestFixture, update_context_state_invalid_value_types) {
    // Create StreamdeckContext to test.
    const std::string context_a = "abc123";
    json settings = {{"dcs_id_compare_monitor", "123"},
                     {"dcs_id_compare_condition", "GREATER_THAN"},
                     {"dcs_id_comparison_value", "1.0"}};
    StreamdeckContext streamdeck_context_a(context_a, settings);

    // Send valid game state value which satisfies monitor condition.
    std::string mock_dcs_message = "header*123=2.0";
    mock_dcs.DcsSend(mock_dcs_message);
    dcs_interface.update_dcs_state();
    streamdeck_context_a.updateContextState(dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, context_a);
    EXPECT_EQ(esd_connection_manager.state_, 1);

    // Send game state value as an integer -- should still resolve to float comparison.
    mock_dcs_message = "header*123=20";
    mock_dcs.DcsSend(mock_dcs_message);
    dcs_interface.update_dcs_state();
    streamdeck_context_a.updateContextState(dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.state_, 1);

    // Send game state value with letters -- should treat as string and not try comparison.
    mock_dcs_message = "header*123=20a";
    mock_dcs.DcsSend(mock_dcs_message);
    dcs_interface.update_dcs_state();
    streamdeck_context_a.updateContextState(dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.state_, 0);

    // Send game state as empty -- should treat as string and not try comparison.
    mock_dcs_message = "header*123=";
    mock_dcs.DcsSend(mock_dcs_message);
    dcs_interface.update_dcs_state();
    streamdeck_context_a.updateContextState(dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.state_, 0);

    // Send DCS ID as a float -- should not read update.
    mock_dcs_message = "header*123.0=2.0";
    mock_dcs.DcsSend(mock_dcs_message);
    dcs_interface.update_dcs_state();
    streamdeck_context_a.updateContextState(dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.state_, 0);

    // Send valid game state, but make settings dcs_id monitor value invalid as a float.
    settings["dcs_id_compare_monitor"] = "123.0";
    streamdeck_context_a.updateContextSettings(settings);
    mock_dcs_message = "header*123=2.0";
    mock_dcs.DcsSend(mock_dcs_message);
    dcs_interface.update_dcs_state();
    streamdeck_context_a.updateContextState(dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.state_, 0);

    // Send valid game state, but make settings comparison value invalid as a float.
    settings["dcs_id_compare_monitor"] = "123";
    settings["dcs_id_comparison_value"] = "1.0abc";
    streamdeck_context_a.updateContextSettings(settings);
    mock_dcs_message = "header*123=2.0";
    mock_dcs.DcsSend(mock_dcs_message);
    dcs_interface.update_dcs_state();
    streamdeck_context_a.updateContextState(dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.state_, 0);

    // Use leading spaces, zeros, and integers -- should be compared as floats without problem.
    settings["dcs_id_compare_monitor"] = "  00123";
    settings["dcs_id_comparison_value"] = "  0001.00";
    streamdeck_context_a.updateContextSettings(settings);
    mock_dcs_message = "header*123=   002";
    mock_dcs.DcsSend(mock_dcs_message);
    dcs_interface.update_dcs_state();
    streamdeck_context_a.updateContextState(dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.state_, 1);

    // A trailing space is currently not handled.
    // TODO: Consider stripping trailing spaces.
    settings["dcs_id_compare_monitor"] = "123";
    settings["dcs_id_comparison_value"] = "1.0 "; //< Trailing space
    streamdeck_context_a.updateContextSettings(settings);
    mock_dcs_message = "header*123=2.0";
    mock_dcs.DcsSend(mock_dcs_message);
    dcs_interface.update_dcs_state();
    streamdeck_context_a.updateContextState(dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.state_, 0);
}

TEST_F(StreamdeckContextTestFixture, class_instances_within_container) {
    // This is more of a test of the MyStreamDeckPlugin.cpp use of this class than of the class itself.

    std::string mock_dcs_message = "header*1=a:2=b:3=c";
    mock_dcs.DcsSend(mock_dcs_message);
    dcs_interface.update_dcs_state();

    std::unordered_map<std::string, StreamdeckContext> ctx_map;
    ctx_map["ctx_a"] = StreamdeckContext("ctx_a", {{"dcs_id_string_monitor", "1"}});
    ctx_map["ctx_b"] = StreamdeckContext("ctx_b", {{"dcs_id_string_monitor", "2"}});
    ctx_map["ctx_c"] = StreamdeckContext("ctx_c", {{"dcs_id_string_monitor", "3"}});

    // Update state through map key.
    ctx_map["ctx_b"].updateContextState(dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, "ctx_b");
    EXPECT_EQ(esd_connection_manager.title_, "b");

    // Update settings through map key.
    ctx_map["ctx_b"].updateContextSettings({{"dcs_id_string_monitor", "1"}});

    // Test that new settings are reflected in state send.
    ctx_map["ctx_b"].updateContextState(dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, "ctx_b");
    EXPECT_EQ(esd_connection_manager.title_, "a");
}

TEST_F(StreamdeckContextTestFixture, force_send_state_update) {
    StreamdeckContext streamdeck_context("abc123");

    // Test 1 -- With updateContextState and no detected state changes, no state is sent to connection manager.
    streamdeck_context.updateContextState(dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, "");

    // Test -- force send will send current state regardless of state change.
    streamdeck_context.forceSendState(&esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, "abc123");
    EXPECT_EQ(esd_connection_manager.state_, 0);
}
