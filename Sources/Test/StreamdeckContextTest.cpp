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

#include "../DcsInterface/StreamdeckContext.cpp"

TEST(StreamdeckContextTest, update_context_state_when_no_dcs) {
    // Test -- With an unpopulated game state in dcs_interface, try to update context state.
    StreamdeckContext test_context("def456");
    DcsConnectionSettings connection_settings = {"2304", "2305", "127.0.0.1"};
    DcsInterface dcs_interface(connection_settings);
    ESDConnectionManager esd_connection_manager;
    test_context.updateContextState(&dcs_interface, &esd_connection_manager);
    // Expect no state or title change as default context state and title values have not changed.
    EXPECT_EQ(esd_connection_manager.context_, "");
}

class StreamdeckContextTestFixture : public ::testing::Test {
  public:
    StreamdeckContextTestFixture()
        : // Mock DCS socket uses the reverse rx and tx ports of dcs_interface so it can communicate with it.
          mock_dcs(connection_settings.tx_port, connection_settings.rx_port, connection_settings.ip_address),
          dcs_interface(connection_settings), fixture_context(fixture_context_id) {

        // Consume intial reset command sent to to mock_dcs.
        (void)mock_dcs.DcsReceive();

        // Send a single message from mock DCS that contains update for monitored ID.
        std::string mock_dcs_message = "header*761=1:765=2.00:2026=TEXT_STR:2027=0.1";
        mock_dcs.DcsSend(mock_dcs_message);
        dcs_interface.update_dcs_state();
    }

    DcsConnectionSettings connection_settings = {"1908", "1909", "127.0.0.1"};
    DcsSocket mock_dcs;                          // A socket that will mock Send/Receive messages from DCS.
    DcsInterface dcs_interface;                  // DCS Interface to test.
    ESDConnectionManager esd_connection_manager; // Streamdeck connection manager, using mock class definition.
    StreamdeckContext fixture_context;

    // Create StreamdeckContext to test without any defined settings.
    static inline std::string fixture_context_id = "abc123";
};

TEST_F(StreamdeckContextTestFixture, update_context_state_dcs_id_compare) {
    // Create StreamdeckContext initialized with settings to test.
    const std::string context_id = "def456";
    const json settings = {{"dcs_id_compare_monitor", "765"},
                           {"dcs_id_compare_condition", "EQUAL_TO"},
                           {"dcs_id_comparison_value", "2.0"}};
    StreamdeckContext test_context(context_id, settings);
    test_context.updateContextState(&dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, context_id);
    EXPECT_EQ(esd_connection_manager.state_, 1);
}

TEST_F(StreamdeckContextTestFixture, update_context_state_string_monitor_passthrough) {
    // Create StreamdeckContext initialized with settings to test.
    const std::string context_id = "def456";
    const json settings = {{"dcs_id_string_monitor", "2026"}, {"string_monitor_passthrough_check", true}};
    StreamdeckContext test_context(context_id, settings);
    test_context.updateContextState(&dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, context_id);
    EXPECT_EQ(esd_connection_manager.title_, "TEXT_STR");
}

TEST_F(StreamdeckContextTestFixture, update_context_state_string_monitor_vertical_spacing_positive) {
    // Create StreamdeckContext initialized with settings to test.
    const std::string context_id = "def456";
    const json settings = {{"dcs_id_string_monitor", "2026"},
                           {"string_monitor_vertical_spacing", "2"},
                           {"string_monitor_passthrough_check", true}};
    StreamdeckContext test_context(context_id, settings);
    test_context.updateContextState(&dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, context_id);
    EXPECT_EQ(esd_connection_manager.title_, "TEXT_STR\n\n");
}

TEST_F(StreamdeckContextTestFixture, update_context_state_string_monitor_vertical_spacing_negative) {
    // Create StreamdeckContext initialized with settings to test.
    const std::string context_id = "def456";
    const json settings = {{"dcs_id_string_monitor", "2026"},
                           {"string_monitor_vertical_spacing", "-4"},
                           {"string_monitor_passthrough_check", true}};
    StreamdeckContext test_context(context_id, settings);
    test_context.updateContextState(&dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, context_id);
    EXPECT_EQ(esd_connection_manager.title_, "\n\n\n\nTEXT_STR");
}

TEST_F(StreamdeckContextTestFixture, update_context_state_string_monitor_mapping) {
    // Create StreamdeckContext initialized with settings to test.
    const std::string context_id = "def456";
    const json settings = {{"dcs_id_string_monitor", "2027"},
                           {"string_monitor_passthrough_check", false},
                           {"string_monitor_mapping", "0.0=A,0.1=B,0.2=C"}};
    StreamdeckContext fixture_context(context_id, settings);
    fixture_context.updateContextState(&dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, context_id);
    EXPECT_EQ(esd_connection_manager.title_, "B");
}

TEST_F(StreamdeckContextTestFixture, update_context_state_string_monitor_mapping_unknown_key) {
    // Create StreamdeckContext initialized with settings to test.
    const std::string context_id = "def456";
    const json settings = {{"dcs_id_string_monitor", "2027"},
                           {"string_monitor_passthrough_check", false},
                           {"string_monitor_mapping", "0.0=A,0.1=B,0.2=C"}};
    StreamdeckContext test_context(context_id, settings);
    test_context.updateContextState(&dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, context_id);
    EXPECT_EQ(esd_connection_manager.title_, "B");

    // Test -- Check that an empty title string is sent when an unmapped key is received from game.
    std::string new_mock_dcs_message = "header*2027=0.6";
    mock_dcs.DcsSend(new_mock_dcs_message);
    dcs_interface.update_dcs_state();
    test_context.updateContextState(&dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, context_id);
    EXPECT_EQ(esd_connection_manager.title_, "");
}

TEST_F(StreamdeckContextTestFixture, update_context_settings) {
    // Test 1 -- With no settings defined, streamdeck context should not send update.
    fixture_context.updateContextState(&dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, "");

    // Test 2 -- With populated settings, streamdeck context should try to update context state.
    json settings = {{"dcs_id_compare_monitor", "765"},
                     {"dcs_id_compare_condition", "EQUAL_TO"},
                     {"dcs_id_comparison_value", "2.0"},
                     {"dcs_id_string_monitor", "2026"},
                     {"string_monitor_passthrough_check", true}};
    fixture_context.updateContextSettings(settings);
    fixture_context.updateContextState(&dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, fixture_context_id);
    EXPECT_EQ(esd_connection_manager.state_, 1);
    EXPECT_EQ(esd_connection_manager.title_, "TEXT_STR");

    // Test 3 -- With cleared settings, streamdeck context should send default state and title.
    settings = {{"dcs_id_compare_monitor", ""},
                {"dcs_id_compare_condition", "EQUAL_TO"}, //< selection type always has some value.
                {"dcs_id_comparison_value", ""},
                {"dcs_id_string_monitor", ""}};
    fixture_context.updateContextSettings(settings);
    fixture_context.updateContextState(&dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, fixture_context_id);
    EXPECT_EQ(esd_connection_manager.state_, 0);
    EXPECT_EQ(esd_connection_manager.title_, "");
}

class StreamdeckContextComparisonTestFixture : public StreamdeckContextTestFixture {
  public:
    StreamdeckContextComparisonTestFixture()
        : // Create StreamdeckContexts with different comparison selections to test.
          context_with_equals("ctx_equals",
                              {{"dcs_id_compare_monitor", "123"},
                               {"dcs_id_compare_condition", "EQUAL_TO"},
                               {"dcs_id_comparison_value", std::to_string(comparison_value)}}),
          context_with_less_than("ctx_less_than",
                                 {{"dcs_id_compare_monitor", "123"},
                                  {"dcs_id_compare_condition", "LESS_THAN"},
                                  {"dcs_id_comparison_value", std::to_string(comparison_value)}}),
          context_with_greater_than("ctx_greater_than",
                                    {{"dcs_id_compare_monitor", "123"},
                                     {"dcs_id_compare_condition", "GREATER_THAN"},
                                     {"dcs_id_comparison_value", std::to_string(comparison_value)}}) {}

    const float comparison_value = 1.56F;
    StreamdeckContext context_with_equals;
    StreamdeckContext context_with_less_than;
    StreamdeckContext context_with_greater_than;
};

TEST_F(StreamdeckContextComparisonTestFixture, dcs_id_float_compare_to_zero) {
    // Test -- Compare 0 to non-zero comparison_value.
    const std::string mock_dcs_message = "header*123=0";
    mock_dcs.DcsSend(mock_dcs_message);
    dcs_interface.update_dcs_state();

    esd_connection_manager.clear_buffer();
    context_with_equals.updateContextState(&dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, "");
    esd_connection_manager.clear_buffer();
    context_with_less_than.updateContextState(&dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, "ctx_less_than");
    EXPECT_EQ(esd_connection_manager.state_, 1);
    esd_connection_manager.clear_buffer();
    context_with_greater_than.updateContextState(&dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, "");
}

TEST_F(StreamdeckContextComparisonTestFixture, dcs_id_float_compare_to_lesser_value) {
    // Test -- Compare value less than comparison_value.
    const std::string mock_dcs_message = "header*123=" + std::to_string(comparison_value / 2.0F);
    mock_dcs.DcsSend(mock_dcs_message);
    dcs_interface.update_dcs_state();

    esd_connection_manager.clear_buffer();
    context_with_equals.updateContextState(&dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, "");
    esd_connection_manager.clear_buffer();
    context_with_less_than.updateContextState(&dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, "ctx_less_than");
    esd_connection_manager.clear_buffer();
    context_with_greater_than.updateContextState(&dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, "");
}

TEST_F(StreamdeckContextComparisonTestFixture, dcs_id_float_compare_to_equal_value) {
    // Test -- Compare value equal to comparison_value.
    const std::string mock_dcs_message = "header*123=" + std::to_string(comparison_value);
    mock_dcs.DcsSend(mock_dcs_message);
    dcs_interface.update_dcs_state();

    esd_connection_manager.clear_buffer();
    context_with_equals.updateContextState(&dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, "ctx_equals");
    EXPECT_EQ(esd_connection_manager.state_, 1);
    esd_connection_manager.clear_buffer();
    context_with_less_than.updateContextState(&dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, "");
    EXPECT_EQ(esd_connection_manager.state_, 0);
    esd_connection_manager.clear_buffer();
    context_with_greater_than.updateContextState(&dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, "");
}

TEST_F(StreamdeckContextComparisonTestFixture, dcs_id_float_compare_to_greater_value) {
    // Test -- Compare value greater than comparison_value.
    const std::string mock_dcs_message = "header*123=" + std::to_string(comparison_value * 2.0F);
    mock_dcs.DcsSend(mock_dcs_message);
    dcs_interface.update_dcs_state();

    esd_connection_manager.clear_buffer();
    context_with_equals.updateContextState(&dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, "");
    EXPECT_EQ(esd_connection_manager.state_, 0);
    esd_connection_manager.clear_buffer();
    context_with_less_than.updateContextState(&dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, "");
    esd_connection_manager.clear_buffer();
    context_with_greater_than.updateContextState(&dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, "ctx_greater_than");
    EXPECT_EQ(esd_connection_manager.state_, 1);
}

// Test comparison to invalid values.

TEST_F(StreamdeckContextComparisonTestFixture, dcs_id_float_compare_to_integer) {
    // Send game state value as an integer -- should still resolve to float comparison.
    const std::string mock_dcs_message = "header*123=20";
    mock_dcs.DcsSend(mock_dcs_message);
    dcs_interface.update_dcs_state();
    context_with_greater_than.updateContextState(&dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.state_, 1);
}
TEST_F(StreamdeckContextComparisonTestFixture, dcs_id_float_compare_to_alphanumeric) {
    // Send game state value with letters -- should treat as string and not try comparison.
    const std::string mock_dcs_message = "header*123=20a";
    mock_dcs.DcsSend(mock_dcs_message);
    dcs_interface.update_dcs_state();
    context_with_greater_than.updateContextState(&dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.state_, 0);
}
TEST_F(StreamdeckContextComparisonTestFixture, dcs_id_float_compare_to_empty) {

    // Send game state as empty -- should treat as string and not try comparison.
    const std::string mock_dcs_message = "header*123=";
    mock_dcs.DcsSend(mock_dcs_message);
    dcs_interface.update_dcs_state();
    context_with_greater_than.updateContextState(&dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.state_, 0);
}
TEST_F(StreamdeckContextComparisonTestFixture, dcs_id_float_compare_to_dcs_id_float) {
    // Send DCS ID as a float -- should not read update.
    const std::string mock_dcs_message = "header*123.0=2.0";
    mock_dcs.DcsSend(mock_dcs_message);
    dcs_interface.update_dcs_state();
    context_with_greater_than.updateContextState(&dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.state_, 0);
}
TEST_F(StreamdeckContextComparisonTestFixture, dcs_id_float_compare_with_settings_id_as_float) {
    // Send valid game state, but make settings dcs_id monitor value invalid as a float.
    json settings;
    settings["dcs_id_compare_monitor"] = "123.0";
    context_with_greater_than.updateContextSettings(settings);
    const std::string mock_dcs_message = "header*123=2.0";
    mock_dcs.DcsSend(mock_dcs_message);
    dcs_interface.update_dcs_state();
    context_with_greater_than.updateContextState(&dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.state_, 0);
}
TEST_F(StreamdeckContextComparisonTestFixture, dcs_id_float_compare_with_invalid_settings_comparison_value) {
    // Send valid game state, but make settings comparison value invalid as a float.
    json settings;
    settings["dcs_id_compare_monitor"] = "123";
    settings["dcs_id_comparison_value"] = "1.0abc";
    context_with_greater_than.updateContextSettings(settings);
    const std::string mock_dcs_message = "header*123=2.0";
    mock_dcs.DcsSend(mock_dcs_message);
    dcs_interface.update_dcs_state();
    context_with_greater_than.updateContextState(&dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.state_, 0);
}
TEST_F(StreamdeckContextComparisonTestFixture, dcs_id_float_compare_to_float_with_leading_spaces) {
    // Use leading spaces, zeros, and integers -- should be compared as floats without problem.
    json settings;
    settings["dcs_id_compare_monitor"] = "  00123";
    settings["dcs_id_comparison_value"] = "  0001.00";
    context_with_greater_than.updateContextSettings(settings);
    const std::string mock_dcs_message = "header*123=   002";
    mock_dcs.DcsSend(mock_dcs_message);
    dcs_interface.update_dcs_state();
    context_with_greater_than.updateContextState(&dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.state_, 1);
}
TEST_F(StreamdeckContextComparisonTestFixture, dcs_id_float_compare_to_float_with_trailing_space) {
    json settings;
    settings["dcs_id_compare_monitor"] = "123";
    settings["dcs_id_comparison_value"] = "1.0 "; //< Trailing space
    context_with_greater_than.updateContextSettings(settings);
    const std::string mock_dcs_message = "header*123=2.0";
    mock_dcs.DcsSend(mock_dcs_message);
    dcs_interface.update_dcs_state();
    context_with_greater_than.updateContextState(&dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.state_, 1);
}

TEST_F(StreamdeckContextTestFixture, force_send_state_update) {
    // Test 1 -- With updateContextState and no detected state changes, no state is sent to connection manager.
    fixture_context.updateContextState(&dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, "");

    // Test -- force send will send current state regardless of state change.
    fixture_context.forceSendState(&esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, "abc123");
    EXPECT_EQ(esd_connection_manager.state_, 0);
}

class StreamdeckContextKeyPressTestFixture : public StreamdeckContextTestFixture {
  public:
    StreamdeckContextKeyPressTestFixture()
        : // Create default json payload.
          payload({{"state", 0},
                   {"settings",
                    {{"button_id", std::to_string(button_id)},
                     {"device_id", device_id},
                     {"press_value", press_value},
                     {"release_value", release_value},
                     {"disable_release_check", false},
                     {"send_when_first_state_value", send_when_first_state_value},
                     {"send_when_second_state_value", send_when_second_state_value},
                     {"dcs_id_increment_monitor", dcs_id_increment_monitor},
                     {"increment_value", increment_value},
                     {"increment_min", increment_min},
                     {"increment_max", increment_max},
                     {"increment_cycle_allowed_check", false}}}}) {
        fixture_context.updateContextSettings(payload["settings"]);
    }

    int button_id = 2;
    std::string device_id = "23";
    std::string press_value = "4";
    std::string release_value = "5";
    std::string send_when_first_state_value = "6";
    std::string send_when_second_state_value = "7";
    std::string dcs_id_increment_monitor = "321";
    std::string increment_value = "0.1";
    std::string increment_min = "0";
    std::string increment_max = "1";
    json payload;
};

TEST_F(StreamdeckContextKeyPressTestFixture, handle_invalid_button_id) {
    payload["settings"]["button_id"] = "abc";
    const std::string action = "com.ctytler.dcs.static.button.one-state";
    fixture_context.handleButtonEvent(&dcs_interface, KEY_DOWN, action, payload);
    const std::stringstream ss_received = mock_dcs.DcsReceive();
    std::string expected_command = "";
    EXPECT_EQ(expected_command, ss_received.str());
}

TEST_F(StreamdeckContextKeyPressTestFixture, handle_invalid_device_id) {
    payload["settings"]["device_id"] = "32.4";
    const std::string action = "com.ctytler.dcs.static.button.one-state";
    fixture_context.handleButtonEvent(&dcs_interface, KEY_DOWN, action, payload);
    const std::stringstream ss_received = mock_dcs.DcsReceive();
    std::string expected_command = "";
    EXPECT_EQ(expected_command, ss_received.str());
}

TEST_F(StreamdeckContextKeyPressTestFixture, handle_keydown_momentary) {
    const std::string action = "com.ctytler.dcs.static.button.one-state";
    fixture_context.handleButtonEvent(&dcs_interface, KEY_DOWN, action, payload);
    const std::stringstream ss_received = mock_dcs.DcsReceive();
    std::string expected_command = "C" + device_id + "," + std::to_string(button_id) + "," + press_value;
    EXPECT_EQ(expected_command, ss_received.str());
}

TEST_F(StreamdeckContextKeyPressTestFixture, handle_keyup_momentary) {
    const std::string action = "com.ctytler.dcs.static.button.one-state";
    fixture_context.handleButtonEvent(&dcs_interface, KEY_UP, action, payload);
    const std::stringstream ss_received = mock_dcs.DcsReceive();
    std::string expected_command = "C" + device_id + "," + std::to_string(button_id) + "," + release_value;
    EXPECT_EQ(expected_command, ss_received.str());
}

TEST_F(StreamdeckContextKeyPressTestFixture, handle_keyup_momentary_release_send_disabled) {
    payload["settings"]["disable_release_check"] = true;
    const std::string action = "com.ctytler.dcs.static.button.one-state";
    fixture_context.handleButtonEvent(&dcs_interface, KEY_UP, action, payload);
    const std::stringstream ss_received = mock_dcs.DcsReceive();
    std::string expected_command = "";
    EXPECT_EQ(expected_command, ss_received.str());
}

TEST_F(StreamdeckContextKeyPressTestFixture, handle_keydown_momentary_empty_value) {
    payload["settings"]["press_value"] = "";
    const std::string action = "com.ctytler.dcs.static.button.one-state";
    fixture_context.handleButtonEvent(&dcs_interface, KEY_DOWN, action, payload);
    const std::stringstream ss_received = mock_dcs.DcsReceive();
    std::string expected_command = "";
    EXPECT_EQ(expected_command, ss_received.str());
}

TEST_F(StreamdeckContextKeyPressTestFixture, handle_keydown_switch_in_first_state) {
    const std::string action = "com.ctytler.dcs.switch.two-state";
    fixture_context.handleButtonEvent(&dcs_interface, KEY_DOWN, action, payload);
    const std::stringstream ss_received = mock_dcs.DcsReceive();
    std::string expected_command =
        "C" + device_id + "," + std::to_string(button_id) + "," + send_when_first_state_value;
    EXPECT_EQ(expected_command, ss_received.str());
}

TEST_F(StreamdeckContextKeyPressTestFixture, handle_keydown_switch_in_second_state) {
    payload["state"] = 1;
    const std::string action = "com.ctytler.dcs.switch.two-state";
    fixture_context.handleButtonEvent(&dcs_interface, KEY_DOWN, action, payload);
    const std::stringstream ss_received = mock_dcs.DcsReceive();
    std::string expected_command =
        "C" + device_id + "," + std::to_string(button_id) + "," + send_when_second_state_value;
    EXPECT_EQ(expected_command, ss_received.str());
}

TEST_F(StreamdeckContextKeyPressTestFixture, handle_keyup_switch) {
    const std::string action = "com.ctytler.dcs.switch.two-state";
    fixture_context.handleButtonEvent(&dcs_interface, KEY_UP, action, payload);
    const std::stringstream ss_received = mock_dcs.DcsReceive();
    // Expect no command sent (empty string is due to mock socket functionality).
    std::string expected_command = "";
    EXPECT_EQ(expected_command, ss_received.str());
}

TEST_F(StreamdeckContextKeyPressTestFixture, handle_keydown_switch_empty_value) {
    payload["settings"]["send_when_first_state_value"] = "";
    const std::string action = "com.ctytler.dcs.switch.two-state";
    fixture_context.handleButtonEvent(&dcs_interface, KEY_DOWN, action, payload);
    const std::stringstream ss_received = mock_dcs.DcsReceive();
    std::string expected_command = "";
    EXPECT_EQ(expected_command, ss_received.str());
}

TEST_F(StreamdeckContextKeyPressTestFixture, handle_keydown_increment) {
    const std::string action = "com.ctytler.dcs.increment.two-state";
    fixture_context.handleButtonEvent(&dcs_interface, KEY_DOWN, action, payload);
    const std::stringstream ss_received = mock_dcs.DcsReceive();
    // Expect no command sent (empty string is due to mock socket functionality).
    std::string expected_command = "C" + device_id + "," + std::to_string(button_id) + "," + increment_value;
    EXPECT_EQ(expected_command, ss_received.str());
}

TEST_F(StreamdeckContextKeyPressTestFixture, handle_keydown_increment_after_external_increment_change) {
    // Receive a value update from DCS game state for increment monitor.
    const std::string external_increment_start = "0.5";
    // Send a single message from mock DCS that contains update for monitored ID.
    std::string mock_dcs_message = "header*" + dcs_id_increment_monitor + "=" + external_increment_start;
    mock_dcs.DcsSend(mock_dcs_message);
    dcs_interface.update_dcs_state();
    fixture_context.updateContextState(&dcs_interface, &esd_connection_manager);

    const std::string action = "com.ctytler.dcs.increment.two-state";
    fixture_context.handleButtonEvent(&dcs_interface, KEY_DOWN, action, payload);
    const std::stringstream ss_received = mock_dcs.DcsReceive();
    const Decimal expected_increment_value = Decimal(external_increment_start) + Decimal(increment_value);
    std::string expected_command =
        "C" + device_id + "," + std::to_string(button_id) + "," + expected_increment_value.str();
    EXPECT_EQ(expected_command, ss_received.str());
}

TEST_F(StreamdeckContextKeyPressTestFixture, handle_keydown_increment_multiple) {
    const std::string action = "com.ctytler.dcs.increment.two-state";
    std::stringstream ss_received;
    for (int i = 0; i < 5; ++i) {
        fixture_context.handleButtonEvent(&dcs_interface, KEY_DOWN, action, payload);
        ss_received = mock_dcs.DcsReceive();
    }
    // Expect no command sent (empty string is due to mock socket functionality).
    std::string expected_command = "C" + device_id + "," + std::to_string(button_id) + "," + "0.5";
    EXPECT_EQ(expected_command, ss_received.str());
}

TEST_F(StreamdeckContextKeyPressTestFixture, handle_keydown_increment_to_max) {
    const std::string action = "com.ctytler.dcs.increment.two-state";
    std::stringstream ss_received;
    for (int i = 0; i < 15; ++i) {
        fixture_context.handleButtonEvent(&dcs_interface, KEY_DOWN, action, payload);
        ss_received = mock_dcs.DcsReceive();
    }
    // Expect no command sent (empty string is due to mock socket functionality).
    std::string expected_command = "C" + device_id + "," + std::to_string(button_id) + "," + increment_max;
    EXPECT_EQ(expected_command, ss_received.str());
}

TEST_F(StreamdeckContextKeyPressTestFixture, handle_keydown_increment_cycle_max_to_min) {
    payload["settings"]["increment_cycle_allowed_check"] = true;
    const std::string action = "com.ctytler.dcs.increment.two-state";
    std::stringstream ss_received;
    for (int i = 0; i < 11; ++i) {
        fixture_context.handleButtonEvent(&dcs_interface, KEY_DOWN, action, payload);
        ss_received = mock_dcs.DcsReceive();
    }
    // Expect no command sent (empty string is due to mock socket functionality).
    std::string expected_command = "C" + device_id + "," + std::to_string(button_id) + "," + increment_min;
    EXPECT_EQ(expected_command, ss_received.str());
}

TEST_F(StreamdeckContextKeyPressTestFixture, handle_keydown_increment_multiple_negative) {
    payload["settings"]["increment_value"] = "-0.1";
    const std::string action = "com.ctytler.dcs.increment.two-state";
    std::stringstream ss_received;
    for (int i = 0; i < 5; ++i) {
        fixture_context.handleButtonEvent(&dcs_interface, KEY_DOWN, action, payload);
        ss_received = mock_dcs.DcsReceive();
    }
    // Expect no command sent (empty string is due to mock socket functionality).
    std::string expected_command = "C" + device_id + "," + std::to_string(button_id) + "," + increment_min;
    EXPECT_EQ(expected_command, ss_received.str());
}

TEST_F(StreamdeckContextKeyPressTestFixture, handle_keydown_increment_negative_to_min) {
    payload["settings"]["increment_value"] = "-0.1";
    const std::string action = "com.ctytler.dcs.increment.two-state";
    std::stringstream ss_received;
    for (int i = 0; i < 15; ++i) {
        fixture_context.handleButtonEvent(&dcs_interface, KEY_DOWN, action, payload);
        ss_received = mock_dcs.DcsReceive();
    }
    // Expect no command sent (empty string is due to mock socket functionality).
    std::string expected_command = "C" + device_id + "," + std::to_string(button_id) + "," + increment_min;
    EXPECT_EQ(expected_command, ss_received.str());
}

TEST_F(StreamdeckContextKeyPressTestFixture, handle_keydown_increment_negative_cycle_min_to_max) {
    payload["settings"]["increment_value"] = "-0.1";
    payload["settings"]["increment_cycle_allowed_check"] = true;
    const std::string action = "com.ctytler.dcs.increment.two-state";
    std::stringstream ss_received;
    for (int i = 0; i < 1; ++i) {
        fixture_context.handleButtonEvent(&dcs_interface, KEY_DOWN, action, payload);
        ss_received = mock_dcs.DcsReceive();
    }
    // Expect no command sent (empty string is due to mock socket functionality).
    std::string expected_command = "C" + device_id + "," + std::to_string(button_id) + "," + increment_max;
    EXPECT_EQ(expected_command, ss_received.str());
}

TEST_F(StreamdeckContextKeyPressTestFixture, handle_keyup_increment) {
    const std::string action = "com.ctytler.dcs.increment.two-state";
    fixture_context.handleButtonEvent(&dcs_interface, KEY_UP, action, payload);
    const std::stringstream ss_received = mock_dcs.DcsReceive();
    // Expect no command sent (empty string is due to mock socket functionality).
    std::string expected_command = "";
    EXPECT_EQ(expected_command, ss_received.str());
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
    ctx_map["ctx_b"].updateContextState(&dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, "ctx_b");
    EXPECT_EQ(esd_connection_manager.title_, "b");

    // Update settings through map key.
    ctx_map["ctx_b"].updateContextSettings({{"dcs_id_string_monitor", "1"}});

    // Test that new settings are reflected in state send.
    ctx_map["ctx_b"].updateContextState(&dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, "ctx_b");
    EXPECT_EQ(esd_connection_manager.title_, "a");
}
