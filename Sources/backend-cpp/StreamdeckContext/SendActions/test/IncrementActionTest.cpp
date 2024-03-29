// Copyright 2021 Charles Tytler

#include "gtest/gtest.h"

#include "SimulatorInterface/SimConnectionManager.h"
#include "StreamdeckContext/SendActions/IncrementAction.h"

#include "Test/MockESDConnectionManager.h"

namespace test
{

class IncrementActionKeyPressTestFixture : public ::testing::Test
{
  public:
    IncrementActionKeyPressTestFixture()
        : // Mock DCS socket uses the reverse rx and tx ports of simulator_interface so it can communicate with it.
          mock_dcs(connection_settings.ip_address, connection_settings.tx_port, connection_settings.rx_port),
          // Create default json payload.
          payload({{"state", 0},
                   {"settings",
                    {{"send_address", send_address},
                     {"dcs_id_increment_monitor", dcs_id_increment_monitor},
                     {"increment_value", increment_value},
                     {"increment_min", increment_min},
                     {"increment_max", increment_max},
                     {"increment_cycle_allowed_check", false}}}})
    {
        sim_connection_manager.connect_to_protocol(Protocol::DCS_ExportScript, connection_settings);
        simulator_interface = sim_connection_manager.get_interface(Protocol::DCS_ExportScript);
        // Consume intial reset command sent to to mock_dcs.
        (void)mock_dcs.receive_stream();
    }

    std::string send_address = "23,2";
    std::string dcs_id_increment_monitor = "321";
    std::string increment_value = "0.1";
    std::string increment_min = "0";
    std::string increment_max = "1";
    json payload;

    SimulatorConnectionSettings connection_settings = {"1938", "1939", "127.0.0.1"};
    UdpSocket mock_dcs;                              // A socket that will mock Send/Receive messages from DCS.
    MockESDConnectionManager esd_connection_manager; // Streamdeck connection manager, using mock class definition.
    IncrementAction fixture_context;
    SimulatorInterface *simulator_interface; // Simulator Interface to test.
  private:
    SimConnectionManager sim_connection_manager;
};

TEST_F(IncrementActionKeyPressTestFixture, handle_keydown_increment)
{
    fixture_context.handleButtonPressedEvent(simulator_interface, &esd_connection_manager, payload);
    const std::stringstream ss_received = mock_dcs.receive_stream();
    std::string expected_command = "C" + send_address + "," + increment_value;
    EXPECT_EQ(expected_command, ss_received.str());
}

TEST_F(IncrementActionKeyPressTestFixture, handle_keydown_increment_after_external_increment_change)
{
    // Receive a value update from DCS game state for increment monitor.
    const std::string external_increment_start = "0.5";
    // Send a single message from mock DCS that contains update for monitored ID.
    std::string mock_dcs_message = "header*" + dcs_id_increment_monitor + "=" + external_increment_start;
    mock_dcs.send_string(mock_dcs_message);
    simulator_interface->update_simulator_state();

    fixture_context.handleButtonPressedEvent(simulator_interface, &esd_connection_manager, payload);
    const std::stringstream ss_received = mock_dcs.receive_stream();
    const Decimal expected_increment_value = Decimal(external_increment_start) + Decimal(increment_value);
    std::string expected_command = "C" + send_address + "," + expected_increment_value.str();
    EXPECT_EQ(expected_command, ss_received.str());
}

TEST_F(IncrementActionKeyPressTestFixture, handle_keyup_increment)
{
    fixture_context.handleButtonReleasedEvent(simulator_interface, &esd_connection_manager, payload);
    const std::stringstream ss_received = mock_dcs.receive_stream();
    // Expect no command sent (empty string is due to mock socket functionality).
    std::string expected_command = "";
    EXPECT_EQ(expected_command, ss_received.str());
}
} // namespace test
