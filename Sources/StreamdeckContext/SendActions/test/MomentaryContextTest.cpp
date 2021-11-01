// Copyright 2021 Charles Tytler

#include "gtest/gtest.h"

#include "StreamdeckContext/SendActions/MomentaryContext.h"

namespace test
{

class MomentaryContextKeyPressTestFixture : public ::testing::Test
{
  public:
    MomentaryContextKeyPressTestFixture()
        : // Mock DCS socket uses the reverse rx and tx ports of dcs_interface so it can communicate with it.
          mock_dcs(connection_settings.ip_address, connection_settings.tx_port, connection_settings.rx_port),
          dcs_interface(connection_settings), fixture_context(fixture_context_id),
          // Create default json payload.
          payload({{"state", 0},
                   {"settings",
                    {{"button_id", button_id},
                     {"device_id", device_id},
                     {"press_value", press_value},
                     {"release_value", release_value},
                     {"disable_release_check", false}}}})
    {
        // Consume intial reset command sent to to mock_dcs.
        (void)mock_dcs.receive();
    }
    DcsConnectionSettings connection_settings = {"1928", "1929", "127.0.0.1"};
    UdpSocket mock_dcs;         // A socket that will mock Send/Receive messages from DCS.
    DcsInterface dcs_interface; // DCS Interface to test.
    std::string fixture_context_id = "abc123";
    MomentaryContext fixture_context;

    std::string button_id = "2";
    std::string device_id = "23";
    std::string press_value = "4";
    std::string release_value = "5";
    json payload;
};

TEST_F(MomentaryContextKeyPressTestFixture, handle_invalid_button_id)
{
    payload["settings"]["button_id"] = "abc";
    fixture_context.handleButtonEvent(dcs_interface, KeyEvent::PRESSED, payload);
    const std::stringstream ss_received = mock_dcs.receive();
    std::string expected_command = "";
    EXPECT_EQ(expected_command, ss_received.str());
}

TEST_F(MomentaryContextKeyPressTestFixture, handle_invalid_device_id)
{
    payload["settings"]["device_id"] = "32.4";
    fixture_context.handleButtonEvent(dcs_interface, KeyEvent::PRESSED, payload);
    const std::stringstream ss_received = mock_dcs.receive();
    std::string expected_command = "";
    EXPECT_EQ(expected_command, ss_received.str());
}

TEST_F(MomentaryContextKeyPressTestFixture, handle_keydown_momentary)
{
    MomentaryContext fc("abc", {});
    fc.handleButtonEvent(dcs_interface, KeyEvent::PRESSED, payload);
    const std::stringstream ss_received = mock_dcs.receive();
    std::string expected_command = "C" + device_id + "," + button_id + "," + press_value;
    EXPECT_EQ(expected_command, ss_received.str());
}

TEST_F(MomentaryContextKeyPressTestFixture, handle_keyup_momentary)
{
    fixture_context.handleButtonEvent(dcs_interface, KeyEvent::RELEASED, payload);
    const std::stringstream ss_received = mock_dcs.receive();
    std::string expected_command = "C" + device_id + "," + button_id + "," + release_value;
    EXPECT_EQ(expected_command, ss_received.str());
}

TEST_F(MomentaryContextKeyPressTestFixture, handle_keyup_momentary_release_send_disabled)
{
    payload["settings"]["disable_release_check"] = true;
    fixture_context.handleButtonEvent(dcs_interface, KeyEvent::RELEASED, payload);
    const std::stringstream ss_received = mock_dcs.receive();
    std::string expected_command = "";
    EXPECT_EQ(expected_command, ss_received.str());
}

TEST_F(MomentaryContextKeyPressTestFixture, handle_keydown_momentary_empty_value)
{
    payload["settings"]["press_value"] = "";
    fixture_context.handleButtonEvent(dcs_interface, KeyEvent::PRESSED, payload);
    const std::stringstream ss_received = mock_dcs.receive();
    std::string expected_command = "";
    EXPECT_EQ(expected_command, ss_received.str());
}

} // namespace test