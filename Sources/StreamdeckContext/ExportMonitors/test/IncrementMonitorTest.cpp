// Copyright 2021 Charles Tytler

#include "gtest/gtest.h"

#include "SimulatorInterface/Backends/DcsExportScriptInterface.h"
#include "StreamdeckContext/ExportMonitors/IncrementMonitor.h"

namespace test
{

TEST(IncrementMonitorTest, DefaultConstructor)
{
    IncrementMonitor monitor{};
    EXPECT_EQ(Decimal("0"), monitor.get_increment_after_command(Decimal("99"), Decimal("99"), Decimal("99"), false));
}

TEST(IncrementMonitorTest, DefaultValueOfZeroWhileIdNotSet)
{
    IncrementMonitor monitor{{{"dcs_id_increment_monitor", ""}}};
    EXPECT_EQ(Decimal("0"), monitor.get_increment_after_command(Decimal("99"), Decimal("99"), Decimal("99"), false));
}

TEST(IncrementMonitorTest, IncrementFromZeroByDefault)
{
    IncrementMonitor monitor{{{"dcs_id_increment_monitor", "123"}}};
    const Decimal delta{"0.1"};
    const Decimal min{"0"};
    const Decimal max{"1"};
    const bool cycling_is_allowed = false;

    EXPECT_EQ(Decimal("0.1"), monitor.get_increment_after_command(delta, min, max, cycling_is_allowed));
}

TEST(IncrementMonitorTest, IncrementFromBelowRange)
{
    IncrementMonitor monitor{{{"dcs_id_increment_monitor", "123"}}};
    const Decimal delta{"1"};
    const Decimal min{"10"};
    const Decimal max{"20"};
    const bool cycling_is_allowed = false;

    EXPECT_EQ(Decimal("10"), monitor.get_increment_after_command(delta, min, max, cycling_is_allowed));
}

TEST(IncrementMonitorTest, IncrementFromAboveRange)
{
    IncrementMonitor monitor{{{"dcs_id_increment_monitor", "123"}}};
    const Decimal delta{"1"};
    const Decimal min{"-20"};
    const Decimal max{"-10"};
    const bool cycling_is_allowed = false;

    EXPECT_EQ(Decimal("-10"), monitor.get_increment_after_command(delta, min, max, cycling_is_allowed));
}

class IncrementMonitorTestFixture : public ::testing::Test
{
  public:
    IncrementMonitorTestFixture()
        : mock_dcs(connection_settings.ip_address, connection_settings.tx_port, connection_settings.rx_port),
          simulator_interface(connection_settings)
    {
        // Consume intial reset command sent to to mock_dcs.
        (void)mock_dcs.receive();
    }

    void set_current_dcs_id_value(const std::string value)
    {
        mock_dcs.send("header*" + monitor_id_value + "=" + value);
        simulator_interface.update_simulator_state();
    }

    std::string monitor_id_value = "123";
    SimulatorConnectionSettings connection_settings{"1908", "1909", "127.0.0.1"};
    UdpSocket mock_dcs;                           // A socket that will mock Send/Receive messages from DCS.
    DcsExportScriptInterface simulator_interface; // Simulator Interface to test.
};

TEST_F(IncrementMonitorTestFixture, CurrentValueIsUpdatedFromDcsInterface)
{
    IncrementMonitor monitor{{{"dcs_id_increment_monitor", monitor_id_value}}};
    set_current_dcs_id_value("0.5");
    monitor.update(simulator_interface);
    const Decimal delta{"0"};
    const Decimal min{"0"};
    const Decimal max{"1"};
    const bool cycling_is_allowed = false;

    EXPECT_EQ(Decimal("0.5"), monitor.get_increment_after_command(delta, min, max, cycling_is_allowed));
}

TEST_F(IncrementMonitorTestFixture, CurrentValueUpdateIsClampedToRange)
{
    IncrementMonitor monitor{{{"dcs_id_increment_monitor", monitor_id_value}}};
    set_current_dcs_id_value("0.5");
    monitor.update(simulator_interface);
    const Decimal delta{"0"};
    const Decimal min{"10"};
    const Decimal max{"20"};
    const bool cycling_is_allowed = false;

    EXPECT_EQ(Decimal("10"), monitor.get_increment_after_command(delta, min, max, cycling_is_allowed));
}

TEST_F(IncrementMonitorTestFixture, IncrementAfterExternalChange)
{
    IncrementMonitor monitor{{{"dcs_id_increment_monitor", monitor_id_value}}};
    set_current_dcs_id_value("-0.5");
    monitor.update(simulator_interface);
    const Decimal delta{"0.1"};
    const Decimal min{"-1"};
    const Decimal max{"1"};
    const bool cycling_is_allowed = false;

    EXPECT_EQ(Decimal("-0.4"), monitor.get_increment_after_command(delta, min, max, cycling_is_allowed));
}

TEST_F(IncrementMonitorTestFixture, IncrementMultipleTimes)
{
    IncrementMonitor monitor{{{"dcs_id_increment_monitor", monitor_id_value}}};
    set_current_dcs_id_value("0");
    monitor.update(simulator_interface);
    const Decimal delta{"0.1"};
    const Decimal min{"-1"};
    const Decimal max{"1"};
    const bool cycling_is_allowed = false;

    const Decimal expected{"0.5"};
    Decimal actual;
    for (int i = 0; i < 5; i++) {
        actual = monitor.get_increment_after_command(delta, min, max, cycling_is_allowed);
    }
    EXPECT_EQ(expected, actual);
}

TEST_F(IncrementMonitorTestFixture, IncrementMultipleTimesNegative)
{
    IncrementMonitor monitor{{{"dcs_id_increment_monitor", monitor_id_value}}};
    set_current_dcs_id_value("0");
    monitor.update(simulator_interface);
    const Decimal delta{"-0.1"};
    const Decimal min{"-1"};
    const Decimal max{"1"};
    const bool cycling_is_allowed = false;

    const Decimal expected{"-0.5"};
    Decimal actual;
    for (int i = 0; i < 5; i++) {
        actual = monitor.get_increment_after_command(delta, min, max, cycling_is_allowed);
    }
    EXPECT_EQ(expected, actual);
}

TEST_F(IncrementMonitorTestFixture, IncrementToMax)
{
    IncrementMonitor monitor{{{"dcs_id_increment_monitor", monitor_id_value}}};
    set_current_dcs_id_value("0.9");
    monitor.update(simulator_interface);
    const Decimal delta{"0.1"};
    const Decimal min{"-1"};
    const Decimal max{"1"};
    const bool cycling_is_allowed = false;

    EXPECT_EQ(Decimal("1.0"), monitor.get_increment_after_command(delta, min, max, cycling_is_allowed));
}

TEST_F(IncrementMonitorTestFixture, IncrementBeyondMaxCyclingOff)
{
    IncrementMonitor monitor{{{"dcs_id_increment_monitor", monitor_id_value}}};
    set_current_dcs_id_value("0.9");
    monitor.update(simulator_interface);
    const Decimal delta{"0.2"};
    const Decimal min{"-1"};
    const Decimal max{"1"};
    const bool cycling_is_allowed = false;

    EXPECT_EQ(Decimal("1.0"), monitor.get_increment_after_command(delta, min, max, cycling_is_allowed));
}

TEST_F(IncrementMonitorTestFixture, IncrementBeyondMaxCyclingOn)
{
    IncrementMonitor monitor{{{"dcs_id_increment_monitor", monitor_id_value}}};
    set_current_dcs_id_value("0.9");
    monitor.update(simulator_interface);
    const Decimal delta{"0.2"};
    const Decimal min{"-1"};
    const Decimal max{"1"};
    const bool cycling_is_allowed = true;

    EXPECT_EQ(Decimal("-1.0"), monitor.get_increment_after_command(delta, min, max, cycling_is_allowed));
}

TEST_F(IncrementMonitorTestFixture, IncrementToMin)
{
    IncrementMonitor monitor{{{"dcs_id_increment_monitor", monitor_id_value}}};
    set_current_dcs_id_value("-0.9");
    monitor.update(simulator_interface);
    const Decimal delta{"-0.1"};
    const Decimal min{"-1"};
    const Decimal max{"1"};
    const bool cycling_is_allowed = false;

    EXPECT_EQ(Decimal("-1.0"), monitor.get_increment_after_command(delta, min, max, cycling_is_allowed));
}

TEST_F(IncrementMonitorTestFixture, IncrementBeyondMinCyclingOff)
{
    IncrementMonitor monitor{{{"dcs_id_increment_monitor", monitor_id_value}}};
    set_current_dcs_id_value("-0.9");
    monitor.update(simulator_interface);
    const Decimal delta{"-0.2"};
    const Decimal min{"-1"};
    const Decimal max{"1"};
    const bool cycling_is_allowed = false;

    EXPECT_EQ(Decimal("-1.0"), monitor.get_increment_after_command(delta, min, max, cycling_is_allowed));
}

TEST_F(IncrementMonitorTestFixture, IncrementBeyondMinCyclingOn)
{
    IncrementMonitor monitor{{{"dcs_id_increment_monitor", monitor_id_value}}};
    set_current_dcs_id_value("-0.9");
    monitor.update(simulator_interface);
    const Decimal delta{"-0.2"};
    const Decimal min{"-1"};
    const Decimal max{"1"};
    const bool cycling_is_allowed = true;

    EXPECT_EQ(Decimal("1.0"), monitor.get_increment_after_command(delta, min, max, cycling_is_allowed));
}

} // namespace test
