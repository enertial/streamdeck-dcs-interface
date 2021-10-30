// Copyright 2021 Charles Tytler

#include "gtest/gtest.h"

#define UNIT_TEST
// Mocked definition.
class ESDConnectionManager
{
};

#include "../StreamdeckContext/SendActions/SwitchContext.cpp"

namespace test
{

class StreamdeckContextTestFixture : public ::testing::Test
{
  public:
    StreamdeckContextTestFixture()
        : // Mock DCS socket uses the reverse rx and tx ports of dcs_interface so it can communicate with it.
          mock_dcs(connection_settings.ip_address, connection_settings.tx_port, connection_settings.rx_port),
          dcs_interface(connection_settings), fixture_context(fixture_context_id)
    {
        // Consume intial reset command sent to to mock_dcs.
        (void)mock_dcs.receive();
    }

    DcsConnectionSettings connection_settings = {"1908", "1909", "127.0.0.1"};
    UdpSocket mock_dcs;         // A socket that will mock Send/Receive messages from DCS.
    DcsInterface dcs_interface; // DCS Interface to test.
    SwitchContext fixture_context;

    // Create StreamdeckContext to test without any defined settings.
    static inline std::string fixture_context_id = "abc123";
};

class SwitchContextKeyPressTestFixture : public StreamdeckContextTestFixture
{
  public:
    SwitchContextKeyPressTestFixture()
        : // Create default json payload.
          payload({{"state", 0},
                   {"settings",
                    {{"button_id", std::to_string(button_id)},
                     {"device_id", device_id},
                     {"send_when_first_state_value", send_when_first_state_value},
                     {"send_when_second_state_value", send_when_second_state_value}}}})
    {
        fixture_context.updateContextSettings(payload["settings"]);
    }

    int button_id = 2;
    std::string device_id = "23";
    std::string send_when_first_state_value = "6";
    std::string send_when_second_state_value = "7";
    json payload;
};

TEST_F(SwitchContextKeyPressTestFixture, handle_invalid_button_id)
{
    payload["settings"]["button_id"] = "abc";
    fixture_context.handleButtonEvent(dcs_interface, KeyEvent::PRESSED, payload);
    const std::stringstream ss_received = mock_dcs.receive();
    std::string expected_command = "";
    EXPECT_EQ(expected_command, ss_received.str());
}

TEST_F(SwitchContextKeyPressTestFixture, handle_invalid_device_id)
{
    payload["settings"]["device_id"] = "32.4";
    fixture_context.handleButtonEvent(dcs_interface, KeyEvent::PRESSED, payload);
    const std::stringstream ss_received = mock_dcs.receive();
    std::string expected_command = "";
    EXPECT_EQ(expected_command, ss_received.str());
}

TEST_F(SwitchContextKeyPressTestFixture, handle_keyup_switch_in_first_state)
{
    fixture_context.handleButtonEvent(dcs_interface, KeyEvent::RELEASED, payload);
    const std::stringstream ss_received = mock_dcs.receive();
    std::string expected_command =
        "C" + device_id + "," + std::to_string(button_id) + "," + send_when_first_state_value;
    EXPECT_EQ(expected_command, ss_received.str());
}

TEST_F(SwitchContextKeyPressTestFixture, handle_keyup_switch_in_second_state)
{
    payload["state"] = 1;
    fixture_context.handleButtonEvent(dcs_interface, KeyEvent::RELEASED, payload);
    const std::stringstream ss_received = mock_dcs.receive();
    std::string expected_command =
        "C" + device_id + "," + std::to_string(button_id) + "," + send_when_second_state_value;
    EXPECT_EQ(expected_command, ss_received.str());
}

TEST_F(SwitchContextKeyPressTestFixture, handle_keydown_switch)
{
    fixture_context.handleButtonEvent(dcs_interface, KeyEvent::PRESSED, payload);
    const std::stringstream ss_received = mock_dcs.receive();
    // Expect no command sent (empty string is due to mock socket functionality).
    std::string expected_command = "";
    EXPECT_EQ(expected_command, ss_received.str());
}

TEST_F(SwitchContextKeyPressTestFixture, handle_keyup_switch_empty_value)
{
    payload["settings"]["send_when_first_state_value"] = "";
    fixture_context.handleButtonEvent(dcs_interface, KeyEvent::RELEASED, payload);
    const std::stringstream ss_received = mock_dcs.receive();
    std::string expected_command = "";
    EXPECT_EQ(expected_command, ss_received.str());
}

} // namespace test