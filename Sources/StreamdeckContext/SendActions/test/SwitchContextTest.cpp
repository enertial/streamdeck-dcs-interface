// Copyright 2021 Charles Tytler

#include "gtest/gtest.h"

#include "StreamdeckContext/SendActions/SwitchContext.h"

#include "SimulatorInterface/Derived/DcsExportScriptInterface.h"
#include "Test/MockESDConnectionManager.h"
namespace test
{

class SwitchContextKeyPressTestFixture : public ::testing::Test
{
  public:
    SwitchContextKeyPressTestFixture()
        : // Mock DCS socket uses the reverse rx and tx ports of dcs_interface so it can communicate with it.
          mock_dcs(connection_settings.ip_address, connection_settings.tx_port, connection_settings.rx_port),
          dcs_interface(connection_settings), fixture_context(fixture_context_id),
          // Create default json payload.
          payload({{"state", 0},
                   {"settings",
                    {{"button_id", button_id},
                     {"device_id", device_id},
                     {"send_when_first_state_value", send_when_first_state_value},
                     {"send_when_second_state_value", send_when_second_state_value}}}})
    {
        // Consume intial reset command sent to to mock_dcs.
        (void)mock_dcs.receive();
    }

    SimulatorConnectionSettings connection_settings = {"1948", "1949", "127.0.0.1"};
    UdpSocket mock_dcs;                              // A socket that will mock Send/Receive messages from DCS.
    DcsExportScriptInterface dcs_interface;          // DCS Interface to test.
    MockESDConnectionManager esd_connection_manager; // Streamdeck connection manager, using mock class definition.
    std::string fixture_context_id = "abc123";
    SwitchContext fixture_context;

    std::string button_id = "2";
    std::string device_id = "23";
    std::string send_when_first_state_value = "6";
    std::string send_when_second_state_value = "7";
    json payload;
};

TEST_F(SwitchContextKeyPressTestFixture, handle_invalid_button_id)
{
    payload["settings"]["button_id"] = "abc";
    fixture_context.handleButtonPressedEvent(dcs_interface, &esd_connection_manager, payload);
    const std::stringstream ss_received = mock_dcs.receive();
    std::string expected_command = "";
    EXPECT_EQ(expected_command, ss_received.str());
}

TEST_F(SwitchContextKeyPressTestFixture, handle_invalid_device_id)
{
    payload["settings"]["device_id"] = "32.4";
    fixture_context.handleButtonPressedEvent(dcs_interface, &esd_connection_manager, payload);
    const std::stringstream ss_received = mock_dcs.receive();
    std::string expected_command = "";
    EXPECT_EQ(expected_command, ss_received.str());
}

TEST_F(SwitchContextKeyPressTestFixture, handle_keyup_switch_in_first_state)
{
    fixture_context.handleButtonReleasedEvent(dcs_interface, &esd_connection_manager, payload);
    const std::stringstream ss_received = mock_dcs.receive();
    std::string expected_command = "C" + device_id + "," + button_id + "," + send_when_first_state_value;
    EXPECT_EQ(expected_command, ss_received.str());
}

TEST_F(SwitchContextKeyPressTestFixture, handle_keyup_switch_in_second_state)
{
    payload["state"] = 1;
    fixture_context.handleButtonReleasedEvent(dcs_interface, &esd_connection_manager, payload);
    const std::stringstream ss_received = mock_dcs.receive();
    std::string expected_command = "C" + device_id + "," + button_id + "," + send_when_second_state_value;
    EXPECT_EQ(expected_command, ss_received.str());
}

TEST_F(SwitchContextKeyPressTestFixture, handle_keydown_switch)
{
    fixture_context.handleButtonPressedEvent(dcs_interface, &esd_connection_manager, payload);
    const std::stringstream ss_received = mock_dcs.receive();
    // Expect no command sent (empty string is due to mock socket functionality).
    std::string expected_command = "";
    EXPECT_EQ(expected_command, ss_received.str());
}

TEST_F(SwitchContextKeyPressTestFixture, handle_keyup_switch_empty_value)
{
    payload["settings"]["send_when_first_state_value"] = "";
    fixture_context.handleButtonReleasedEvent(dcs_interface, &esd_connection_manager, payload);
    const std::stringstream ss_received = mock_dcs.receive();
    std::string expected_command = "";
    EXPECT_EQ(expected_command, ss_received.str());
}

TEST_F(SwitchContextKeyPressTestFixture, handle_keyup_force_send_state_after_delay)
{
    fixture_context.handleButtonReleasedEvent(dcs_interface, &esd_connection_manager, payload);

    // Test that after Button Released event, a forced state update is sent with delay.
    int delay_count = fixture_context.num_frames_delay_forced_state_update_;
    while (delay_count > 0) {
        fixture_context.updateContextState(dcs_interface, &esd_connection_manager);
        EXPECT_EQ(esd_connection_manager.context_, "");
        EXPECT_EQ(esd_connection_manager.num_calls_to_SetState, 0);
        delay_count--;
    }
    fixture_context.updateContextState(dcs_interface, &esd_connection_manager);
    EXPECT_EQ(esd_connection_manager.context_, fixture_context_id);
    EXPECT_EQ(esd_connection_manager.state_, 0);
    EXPECT_EQ(esd_connection_manager.num_calls_to_SetState, 1);
}

} // namespace test