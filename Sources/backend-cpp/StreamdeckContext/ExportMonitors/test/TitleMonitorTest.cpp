// Copyright 2021 Charles Tytler

#include "gtest/gtest.h"

#include "SimulatorInterface/SimConnectionManager.h"
#include "StreamdeckContext/ExportMonitors/TitleMonitor.h"

namespace test
{

class TitleMonitorTestFixture : public ::testing::Test
{
  public:
    TitleMonitorTestFixture()
        : mock_dcs(connection_settings.ip_address, connection_settings.tx_port, connection_settings.rx_port)
    {
        sim_connection_manager.connect_to_protocol(Protocol::DCS_ExportScript, connection_settings);
        simulator_interface = sim_connection_manager.get_interface(Protocol::DCS_ExportScript);
        // Consume intial reset command sent to to mock_dcs.
        (void)mock_dcs.receive_stream();
    }

    void set_current_dcs_id_value(const std::string value)
    {
        mock_dcs.send_string("header*" + monitor_id_value + "=" + value);
        simulator_interface->update_simulator_state();
    }

    std::string monitor_id_value = "123";
    SimulatorConnectionSettings connection_settings{"1908", "1909", "127.0.0.1"};
    UdpSocket mock_dcs;                      // A socket that will mock Send/Receive messages from DCS.
    SimulatorInterface *simulator_interface; // Simulator Interface to test.
  private:
    SimConnectionManager sim_connection_manager;
};

TEST_F(TitleMonitorTestFixture, TitleUsesStringPassthrough)
{
    TitleMonitor monitor{{{"dcs_id_string_monitor", monitor_id_value}, {"string_monitor_passthrough_check", true}}};
    set_current_dcs_id_value("TEXT_STR");
    EXPECT_EQ("TEXT_STR", monitor.determineTitle(simulator_interface));
}

TEST_F(TitleMonitorTestFixture, TitleUsesStringPassthroughDefaultsToTrue)
{
    TitleMonitor monitor{{{"dcs_id_string_monitor", monitor_id_value}}};
    set_current_dcs_id_value("TEXT_STR");
    EXPECT_EQ("TEXT_STR", monitor.determineTitle(simulator_interface));
}

TEST_F(TitleMonitorTestFixture, UpdateVerticalSpacingPositive)
{
    TitleMonitor monitor{{{"dcs_id_string_monitor", monitor_id_value},
                          {"string_monitor_vertical_spacing", "2"},
                          {"string_monitor_passthrough_check", true}}};
    set_current_dcs_id_value("TEXT_STR");
    EXPECT_EQ("TEXT_STR\n\n", monitor.determineTitle(simulator_interface));
}

TEST_F(TitleMonitorTestFixture, UpdateVerticalSpacingNegative)
{
    TitleMonitor monitor{{{"dcs_id_string_monitor", monitor_id_value},
                          {"string_monitor_vertical_spacing", "-4"},
                          {"string_monitor_passthrough_check", true}}};
    set_current_dcs_id_value("TEXT_STR");
    EXPECT_EQ("\n\n\n\nTEXT_STR", monitor.determineTitle(simulator_interface));
}

TEST_F(TitleMonitorTestFixture, StringMonitorMapping)
{
    TitleMonitor monitor{{{"dcs_id_string_monitor", monitor_id_value},
                          {"string_monitor_vertical_spacing", "0"},
                          {"string_monitor_mapping", "0.0=A,0.1=B,0.2=C"},
                          {"string_monitor_passthrough_check", false}}};
    set_current_dcs_id_value("0.0");
    EXPECT_EQ("A", monitor.determineTitle(simulator_interface));
    set_current_dcs_id_value("0.1");
    EXPECT_EQ("B", monitor.determineTitle(simulator_interface));
    set_current_dcs_id_value("0.2");
    EXPECT_EQ("C", monitor.determineTitle(simulator_interface));
}

TEST_F(TitleMonitorTestFixture, StringMonitorMappingUnknownKey)
{
    TitleMonitor monitor{{{"dcs_id_string_monitor", monitor_id_value},
                          {"string_monitor_vertical_spacing", "0"},
                          {"string_monitor_mapping", "0.0=A,0.1=B,0.2=C"},
                          {"string_monitor_passthrough_check", false}}};
    set_current_dcs_id_value("0.3");
    EXPECT_EQ("", monitor.determineTitle(simulator_interface));
}

TEST_F(TitleMonitorTestFixture, SettingsWithEmptyString)
{
    TitleMonitor monitor{{{"dcs_id_string_monitor", ""},
                          {"string_monitor_vertical_spacing", ""},
                          {"string_monitor_mapping", ""},
                          {"string_monitor_passthrough_check", false}}};
    set_current_dcs_id_value("TEXT_STR");
    // Returns a default empty string if settings not set.
    EXPECT_EQ("", monitor.determineTitle(simulator_interface));
}

TEST_F(TitleMonitorTestFixture, UpdateSettings)
{
    TitleMonitor monitor{{{"dcs_id_string_monitor", ""}}};
    set_current_dcs_id_value("TEXT_STR");
    EXPECT_EQ("", monitor.determineTitle(simulator_interface));

    monitor.update_settings({{"dcs_id_string_monitor", monitor_id_value},
                             {"string_monitor_vertical_spacing", "0"},
                             {"string_monitor_mapping", ""},
                             {"string_monitor_passthrough_check", true}});
    EXPECT_EQ("TEXT_STR", monitor.determineTitle(simulator_interface));
}

} // namespace test
