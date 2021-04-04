// Copyright 2021 Charles Tytler

#include "gtest/gtest.h"

#include "../StreamdeckContext/Monitors/ComparisonMonitor.cpp"

namespace test
{

/* COMMENTED OUT - ENABLE BEFORE LANDING

class ComparisonTestFixture : public ::testing::Test
{
  public:
    ComparisonTestFixture()
        : // Create StreamdeckContexts with different comparison selections to test.
          monitor_with_equals({{"dcs_id_compare_monitor", "123"},
                               {"dcs_id_compare_condition", "EQUAL_TO"},
                               {"dcs_id_comparison_value", std::to_string(comparison_value)}}),
          monitor_with_less_than({{"dcs_id_compare_monitor", "123"},
                                  {"dcs_id_compare_condition", "LESS_THAN"},
                                  {"dcs_id_comparison_value", std::to_string(comparison_value)}}),
          monitor_with_greater_than({{"dcs_id_compare_monitor", "123"},
                                     {"dcs_id_compare_condition", "GREATER_THAN"},
                                     {"dcs_id_comparison_value", std::to_string(comparison_value)}})
    {
    }

    const float comparison_value = 1.56F;
    ComparisonMonitor monitor_with_equals;
    ComparisonMonitor monitor_with_less_than;
    ComparisonMonitor monitor_with_greater_than;
};


TEST_F(ComparisonTestFixture, dcs_id_float_compare_to_zero)
{
    // Test -- Compare 0 to non-zero comparison_value.
    const std::string mock_dcs_message = "header*123=0";
    mock_dcs.send(mock_dcs_message);
    dcs_interface.update_dcs_state();

    ComparisonMonitor::ContextState state;
    esd_connection_manager.clear_buffer();
    state = monitor_with_equals.determineContextState(&dcs_interface);
    EXPECT_EQ(esd_connection_manager.context_, "");
    esd_connection_manager.clear_buffer();
    state = monitor_with_less_than.determineContextState(&dcs_interface);
    EXPECT_EQ(esd_connection_manager.context_, "ctx_less_than");
    EXPECT_EQ(esd_connection_manager.state_, 1);
    esd_connection_manager.clear_buffer();
    state = monitor_with_greater_than.determineContextState(&dcs_interface);
    EXPECT_EQ(esd_connection_manager.context_, "");
}

TEST_F(ComparisonTestFixture, dcs_id_float_compare_to_lesser_value)
{
    // Test -- Compare value less than comparison_value.
    const std::string mock_dcs_message = "header*123=" + std::to_string(comparison_value / 2.0F);
    mock_dcs.send(mock_dcs_message);
    dcs_interface.update_dcs_state();

    esd_connection_manager.clear_buffer();
    state = monitor_with_equals.determineContextState(&dcs_interface);
    EXPECT_EQ(esd_connection_manager.context_, "");
    esd_connection_manager.clear_buffer();
    state = monitor_with_less_than.determineContextState(&dcs_interface);
    EXPECT_EQ(esd_connection_manager.context_, "ctx_less_than");
    esd_connection_manager.clear_buffer();
    state = monitor_with_greater_than.determineContextState(&dcs_interface);
    EXPECT_EQ(esd_connection_manager.context_, "");
}

TEST_F(ComparisonTestFixture, dcs_id_float_compare_to_equal_value)
{
    // Test -- Compare value equal to comparison_value.
    const std::string mock_dcs_message = "header*123=" + std::to_string(comparison_value);
    mock_dcs.send(mock_dcs_message);
    dcs_interface.update_dcs_state();

    esd_connection_manager.clear_buffer();
    state = monitor_with_equals.determineContextState(&dcs_interface);
    EXPECT_EQ(esd_connection_manager.context_, "ctx_equals");
    EXPECT_EQ(esd_connection_manager.state_, 1);
    esd_connection_manager.clear_buffer();
    state = monitor_with_less_than.determineContextState(&dcs_interface);
    EXPECT_EQ(esd_connection_manager.context_, "");
    EXPECT_EQ(esd_connection_manager.state_, 0);
    esd_connection_manager.clear_buffer();
    state = monitor_with_greater_than.determineContextState(&dcs_interface);
    EXPECT_EQ(esd_connection_manager.context_, "");
}

TEST_F(ComparisonTestFixture, dcs_id_float_compare_to_greater_value)
{
    // Test -- Compare value greater than comparison_value.
    const std::string mock_dcs_message = "header*123=" + std::to_string(comparison_value * 2.0F);
    mock_dcs.send(mock_dcs_message);
    dcs_interface.update_dcs_state();

    esd_connection_manager.clear_buffer();
    state = monitor_with_equals.determineContextState(&dcs_interface);
    EXPECT_EQ(esd_connection_manager.context_, "");
    EXPECT_EQ(esd_connection_manager.state_, 0);
    esd_connection_manager.clear_buffer();
    state = monitor_with_less_than.determineContextState(&dcs_interface);
    EXPECT_EQ(esd_connection_manager.context_, "");
    esd_connection_manager.clear_buffer();
    state = monitor_with_greater_than.determineContextState(&dcs_interface);
    EXPECT_EQ(esd_connection_manager.context_, "ctx_greater_than");
    EXPECT_EQ(esd_connection_manager.state_, 1);
}

// Test comparison to invalid values.

TEST_F(ComparisonTestFixture, dcs_id_float_compare_to_integer)
{
    // Send game state value as an integer -- should still resolve to float comparison.
    const std::string mock_dcs_message = "header*123=20";
    mock_dcs.send(mock_dcs_message);
    dcs_interface.update_dcs_state();
    state = monitor_with_greater_than.determineContextState(&dcs_interface);
    EXPECT_EQ(esd_connection_manager.state_, 1);
}
TEST_F(ComparisonTestFixture, dcs_id_float_compare_to_alphanumeric)
{
    // Send game state value with letters -- should treat as string and not try comparison.
    const std::string mock_dcs_message = "header*123=20a";
    mock_dcs.send(mock_dcs_message);
    dcs_interface.update_dcs_state();
    state = monitor_with_greater_than.determineContextState(&dcs_interface);
    EXPECT_EQ(esd_connection_manager.state_, 0);
}
TEST_F(ComparisonTestFixture, dcs_id_float_compare_to_empty)
{
    // Send game state as empty -- should treat as string and not try comparison.
    const std::string mock_dcs_message = "header*123=";
    mock_dcs.send(mock_dcs_message);
    dcs_interface.update_dcs_state();
    state = monitor_with_greater_than.determineContextState(&dcs_interface);
    EXPECT_EQ(esd_connection_manager.state_, 0);
}
TEST_F(ComparisonTestFixture, dcs_id_float_compare_to_string_of_spaces)
{
    // Send game state value as string of spaces -- should treat as (non-numeric) string and not try comparison.
    const std::string mock_dcs_message = "header*123=    ";
    mock_dcs.send(mock_dcs_message);
    dcs_interface.update_dcs_state();
    state = monitor_with_greater_than.determineContextState(&dcs_interface);
    EXPECT_EQ(esd_connection_manager.state_, 0);
}
TEST_F(ComparisonTestFixture, dcs_id_float_compare_to_dcs_id_float)
{
    // Send DCS ID as a float -- should not read update.
    const std::string mock_dcs_message = "header*123.0=2.0";
    mock_dcs.send(mock_dcs_message);
    dcs_interface.update_dcs_state();
    state = monitor_with_greater_than.determineContextState(&dcs_interface);
    EXPECT_EQ(esd_connection_manager.state_, 0);
}
TEST_F(ComparisonTestFixture, dcs_id_float_compare_with_settings_id_as_float)
{
    // Send valid game state, but make settings dcs_id monitor value invalid as a float.
    json settings;
    settings["dcs_id_compare_monitor"] = "123.0";
    state = monitor_with_greater_than.updateContextSettings(settings);
    const std::string mock_dcs_message = "header*123=2.0";
    mock_dcs.send(mock_dcs_message);
    dcs_interface.update_dcs_state();
    state = monitor_with_greater_than.determineContextState(&dcs_interface);
    EXPECT_EQ(esd_connection_manager.state_, 0);
}
TEST_F(ComparisonTestFixture, dcs_id_float_compare_with_invalid_settings_comparison_value)
{
    // Send valid game state, but make settings comparison value invalid as a float.
    json settings;
    settings["dcs_id_compare_monitor"] = "123";
    settings["dcs_id_comparison_value"] = "1.0abc";
    state = monitor_with_greater_than.updateContextSettings(settings);
    const std::string mock_dcs_message = "header*123=2.0";
    mock_dcs.send(mock_dcs_message);
    dcs_interface.update_dcs_state();
    state = monitor_with_greater_than.determineContextState(&dcs_interface);
    EXPECT_EQ(esd_connection_manager.state_, 0);
}
TEST_F(ComparisonTestFixture, dcs_id_float_compare_to_float_with_leading_spaces)
{
    // Use leading spaces, zeros, and integers -- should be compared as floats without problem.
    json settings;
    settings["dcs_id_compare_monitor"] = "  00123";
    settings["dcs_id_comparison_value"] = "  0001.00";
    state = monitor_with_greater_than.updateContextSettings(settings);
    const std::string mock_dcs_message = "header*123=   002";
    mock_dcs.send(mock_dcs_message);
    dcs_interface.update_dcs_state();
    state = monitor_with_greater_than.determineContextState(&dcs_interface);
    EXPECT_EQ(esd_connection_manager.state_, 1);
}
TEST_F(ComparisonTestFixture, dcs_id_float_compare_to_float_with_trailing_space)
{
    json settings;
    settings["dcs_id_compare_monitor"] = "123";
    settings["dcs_id_comparison_value"] = "1.0 "; //< Trailing space
    state = monitor_with_greater_than.updateContextSettings(settings);
    const std::string mock_dcs_message = "header*123=2.0";
    mock_dcs.send(mock_dcs_message);
    dcs_interface.update_dcs_state();
    state = monitor_with_greater_than.determineContextState(&dcs_interface);
    EXPECT_EQ(esd_connection_manager.state_, 1);
}
*/

} // namespace test
