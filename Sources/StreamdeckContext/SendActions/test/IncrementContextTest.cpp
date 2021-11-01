// Copyright 2021 Charles Tytler

#include "gtest/gtest.h"

#include "StreamdeckContext/SendActions/IncrementContext.h"

#include "Test/MockESDConnectionManager.h"

namespace test
{

class IncrementContextKeyPressTestFixture : public ::testing::Test
{
  public:
    IncrementContextKeyPressTestFixture()
        : // Mock DCS socket uses the reverse rx and tx ports of dcs_interface so it can communicate with it.
          mock_dcs(connection_settings.ip_address, connection_settings.tx_port, connection_settings.rx_port),
          dcs_interface(connection_settings), fixture_context(fixture_context_id),
          // Create default json payload.
          payload({{"state", 0},
                   {"settings",
                    {{"button_id", button_id},
                     {"device_id", device_id},
                     {"dcs_id_increment_monitor", dcs_id_increment_monitor},
                     {"increment_value", increment_value},
                     {"increment_min", increment_min},
                     {"increment_max", increment_max},
                     {"increment_cycle_allowed_check", false}}}})
    {
        // Consume intial reset command sent to to mock_dcs.
        (void)mock_dcs.receive();
        fixture_context.updateContextSettings(payload["settings"]);
    }
    DcsConnectionSettings connection_settings = {"1938", "1939", "127.0.0.1"};
    UdpSocket mock_dcs;                              // A socket that will mock Send/Receive messages from DCS.
    DcsInterface dcs_interface;                      // DCS Interface to test.
    MockESDConnectionManager esd_connection_manager; // Streamdeck connection manager, using mock class definition.
    std::string fixture_context_id = "abc123";
    IncrementContext fixture_context;

    std::string button_id = "2";
    std::string device_id = "23";
    std::string dcs_id_increment_monitor = "321";
    std::string increment_value = "0.1";
    std::string increment_min = "0";
    std::string increment_max = "1";
    json payload;
};

TEST_F(IncrementContextKeyPressTestFixture, handle_invalid_button_id)
{
    payload["settings"]["button_id"] = "abc";
    fixture_context.handleButtonEvent(dcs_interface, KeyEvent::PRESSED, payload);
    const std::stringstream ss_received = mock_dcs.receive();
    std::string expected_command = "";
    EXPECT_EQ(expected_command, ss_received.str());
}

TEST_F(IncrementContextKeyPressTestFixture, handle_invalid_device_id)
{
    payload["settings"]["device_id"] = "32.4";
    fixture_context.handleButtonEvent(dcs_interface, KeyEvent::PRESSED, payload);
    const std::stringstream ss_received = mock_dcs.receive();
    std::string expected_command = "";
    EXPECT_EQ(expected_command, ss_received.str());
}

TEST_F(IncrementContextKeyPressTestFixture, handle_keydown_increment)
{
    fixture_context.handleButtonEvent(dcs_interface, KeyEvent::PRESSED, payload);
    const std::stringstream ss_received = mock_dcs.receive();
    std::string expected_command = "C" + device_id + "," + button_id + "," + increment_value;
    EXPECT_EQ(expected_command, ss_received.str());
}

TEST_F(IncrementContextKeyPressTestFixture, handle_keydown_increment_after_external_increment_change)
{
    // Receive a value update from DCS game state for increment monitor.
    const std::string external_increment_start = "0.5";
    // Send a single message from mock DCS that contains update for monitored ID.
    std::string mock_dcs_message = "header*" + dcs_id_increment_monitor + "=" + external_increment_start;
    mock_dcs.send(mock_dcs_message);
    dcs_interface.update_dcs_state();
    fixture_context.updateContextState(dcs_interface, &esd_connection_manager);

    fixture_context.handleButtonEvent(dcs_interface, KeyEvent::PRESSED, payload);
    const std::stringstream ss_received = mock_dcs.receive();
    const Decimal expected_increment_value = Decimal(external_increment_start) + Decimal(increment_value);
    std::string expected_command = "C" + device_id + "," + button_id + "," + expected_increment_value.str();
    EXPECT_EQ(expected_command, ss_received.str());
}

TEST_F(IncrementContextKeyPressTestFixture, handle_keyup_increment)
{
    fixture_context.handleButtonEvent(dcs_interface, KeyEvent::RELEASED, payload);
    const std::stringstream ss_received = mock_dcs.receive();
    // Expect no command sent (empty string is due to mock socket functionality).
    std::string expected_command = "";
    EXPECT_EQ(expected_command, ss_received.str());
}

} // namespace test