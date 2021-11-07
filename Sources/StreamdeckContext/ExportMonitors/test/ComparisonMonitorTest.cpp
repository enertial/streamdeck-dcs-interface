// Copyright 2021 Charles Tytler

#include "gtest/gtest.h"

#include "SimulatorInterface/Backends/DcsExportScriptInterface.h"
#include "StreamdeckContext/ExportMonitors/ImageStateMonitor.h"

namespace test
{

class ImageStateMonitorTestFixture : public ::testing::Test
{
  public:
    ImageStateMonitorTestFixture()
        : // Create StreamdeckContexts with different comparison selections to test.
          context_with_equals({{"dcs_id_compare_monitor", "123"},
                               {"dcs_id_compare_condition", "EQUAL_TO"},
                               {"dcs_id_comparison_value", std::to_string(comparison_value)}}),
          context_with_less_than({{"dcs_id_compare_monitor", "123"},
                                  {"dcs_id_compare_condition", "LESS_THAN"},
                                  {"dcs_id_comparison_value", std::to_string(comparison_value)}}),
          context_with_greater_than({{"dcs_id_compare_monitor", "123"},
                                     {"dcs_id_compare_condition", "GREATER_THAN"},
                                     {"dcs_id_comparison_value", std::to_string(comparison_value)}}),
          mock_dcs(connection_settings.ip_address, connection_settings.tx_port, connection_settings.rx_port),
          dcs_interface(connection_settings)
    {
        // Consume intial reset command sent to to mock_dcs.
        (void)mock_dcs.receive();
    }

    void set_current_dcs_id_value(const std::string value)
    {
        mock_dcs.send("header*123=" + value);
        dcs_interface.update_simulator_state();
    }

    const double comparison_value = 1.56;
    ImageStateMonitor context_with_equals;
    ImageStateMonitor context_with_less_than;
    ImageStateMonitor context_with_greater_than;
    SimulatorConnectionSettings connection_settings{"1908", "1909", "127.0.0.1"};
    UdpSocket mock_dcs;                     // A socket that will mock Send/Receive messages from DCS.
    DcsExportScriptInterface dcs_interface; // DCS Interface to test.
};

TEST_F(ImageStateMonitorTestFixture, CompareToZero)
{
    // Zero compared to positive reference settings value.
    set_current_dcs_id_value("0");
    ASSERT_GT(comparison_value, 0);

    EXPECT_EQ(1, context_with_less_than.determineContextState(dcs_interface));
    EXPECT_EQ(0, context_with_equals.determineContextState(dcs_interface));
    EXPECT_EQ(0, context_with_greater_than.determineContextState(dcs_interface));
}

TEST_F(ImageStateMonitorTestFixture, CompareToNegativeValue)
{
    // Received value is less than reference settings value.
    set_current_dcs_id_value(std::to_string(-1 * comparison_value));
    ASSERT_GT(comparison_value, 0);

    EXPECT_EQ(1, context_with_less_than.determineContextState(dcs_interface));
    EXPECT_EQ(0, context_with_equals.determineContextState(dcs_interface));
    EXPECT_EQ(0, context_with_greater_than.determineContextState(dcs_interface));
}

TEST_F(ImageStateMonitorTestFixture, CompareToLesserPositiveValue)
{
    // Received value is less than reference settings value.
    set_current_dcs_id_value(std::to_string(comparison_value / 2.0));

    EXPECT_EQ(1, context_with_less_than.determineContextState(dcs_interface));
    EXPECT_EQ(0, context_with_equals.determineContextState(dcs_interface));
    EXPECT_EQ(0, context_with_greater_than.determineContextState(dcs_interface));
}

TEST_F(ImageStateMonitorTestFixture, CompareToEqualValue)
{
    // Received value is equal to to reference settings value.
    set_current_dcs_id_value(std::to_string(comparison_value));

    EXPECT_EQ(0, context_with_less_than.determineContextState(dcs_interface));
    EXPECT_EQ(1, context_with_equals.determineContextState(dcs_interface));
    EXPECT_EQ(0, context_with_greater_than.determineContextState(dcs_interface));
}

TEST_F(ImageStateMonitorTestFixture, CompareToGreaterValue)
{
    // Received value is less than reference settings value.
    set_current_dcs_id_value(std::to_string(comparison_value * 2.0));

    EXPECT_EQ(0, context_with_less_than.determineContextState(dcs_interface));
    EXPECT_EQ(0, context_with_equals.determineContextState(dcs_interface));
    EXPECT_EQ(1, context_with_greater_than.determineContextState(dcs_interface));
}

TEST_F(ImageStateMonitorTestFixture, UpdateSettings)
{
    // Zero compared to positive reference settings value.
    const std::string modified_reference = "99";
    set_current_dcs_id_value(modified_reference);

    // Verify initial settings show equal comparison not satisfied.
    EXPECT_EQ(0, context_with_equals.determineContextState(dcs_interface));

    // Modify settings
    const json modified_settings = {{"dcs_id_compare_monitor", "123"},
                                    {"dcs_id_compare_condition", "EQUAL_TO"},
                                    {"dcs_id_comparison_value", modified_reference}};
    context_with_equals.update_settings(modified_settings);

    // Verify modified settings show equal comparison IS satisfied.
    EXPECT_EQ(1, context_with_equals.determineContextState(dcs_interface));
}

// Test comparison to invalid values.

TEST_F(ImageStateMonitorTestFixture, CompareFloatToInt)
{
    // Received int value is appropriately compared to a floating point reference.
    set_current_dcs_id_value("20");
    ASSERT_GT(20, comparison_value);

    EXPECT_EQ(0, context_with_less_than.determineContextState(dcs_interface));
    EXPECT_EQ(0, context_with_equals.determineContextState(dcs_interface));
    EXPECT_EQ(1, context_with_greater_than.determineContextState(dcs_interface));
}

TEST_F(ImageStateMonitorTestFixture, CompareToAlphaNumeric)
{
    // Send game state value with letters -- should treat as string and not try comparison.
    set_current_dcs_id_value("20a");
    ASSERT_GT(20, comparison_value);

    // Expect default state (0) returned.
    EXPECT_EQ(0, context_with_less_than.determineContextState(dcs_interface));
    EXPECT_EQ(0, context_with_equals.determineContextState(dcs_interface));
    EXPECT_EQ(0, context_with_greater_than.determineContextState(dcs_interface));
}

TEST_F(ImageStateMonitorTestFixture, CompareToEmptyString)
{
    // Send game state value with letters -- should treat as string and not try comparison.
    set_current_dcs_id_value("");

    // Expect default state (0) returned.
    EXPECT_EQ(0, context_with_less_than.determineContextState(dcs_interface));
    EXPECT_EQ(0, context_with_equals.determineContextState(dcs_interface));
    EXPECT_EQ(0, context_with_greater_than.determineContextState(dcs_interface));
}

TEST_F(ImageStateMonitorTestFixture, CompareToNumberWithSpacesPadding)
{
    // Send game state value with letters -- should treat as string and not try comparison.
    set_current_dcs_id_value("  " + std::to_string(comparison_value) + "  ");

    // Expect default state (0) returned.
    EXPECT_EQ(0, context_with_less_than.determineContextState(dcs_interface));
    EXPECT_EQ(1, context_with_equals.determineContextState(dcs_interface));
    EXPECT_EQ(0, context_with_greater_than.determineContextState(dcs_interface));
}

TEST_F(ImageStateMonitorTestFixture, CompareWithInvalidDcsID)
{
    // Send valid game state, but make settings dcs_id monitor value invalid as a float.
    ImageStateMonitor context_with_flot_id{json{{"dcs_id_compare_monitor", "123.0"},
                                                {"dcs_id_compare_condition", "EQUAL_TO"},
                                                {"dcs_id_comparison_value", std::to_string(comparison_value)}}};

    set_current_dcs_id_value(std::to_string(comparison_value));
    // Expect default state (0) returned.
    EXPECT_EQ(0, context_with_flot_id.determineContextState(dcs_interface));
}

TEST_F(ImageStateMonitorTestFixture, InvalidComparisonValueSetting)
{
    // Send valid game state, but make settings dcs_id monitor value invalid as a float.
    const ImageStateMonitor context_with_flot_id{json{{"dcs_id_compare_monitor", "123"},
                                                      {"dcs_id_compare_condition", "EQUAL_TO"},
                                                      {"dcs_id_comparison_value", "1.0abc"}}};

    set_current_dcs_id_value(std::to_string(comparison_value));
    // Expect default state (0) returned.
    EXPECT_EQ(0, context_with_flot_id.determineContextState(dcs_interface));
}

} // namespace test
