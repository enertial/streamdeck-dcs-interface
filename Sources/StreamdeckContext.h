// Copyright 2020 Charles Tytler

#pragma once

#include "DcsInterface.h"

#ifndef UNIT_TEST
#include "Common/ESDConnectionManager.h"
#endif

#include <string>

class StreamdeckContext
{
public:
    explicit StreamdeckContext(const std::string &context);
    explicit StreamdeckContext(const std::string &context, const json &settings);
    bool operator==(const StreamdeckContext &rhs) const;
    bool operator<(const StreamdeckContext &rhs) const;

    /**
     * @brief Queries the dcs_interface for updates to the Context's monitored DCS IDs.
     * 
     * @param dcs_interface Interface to DCS containing current game state.
     * @param mConnectionManager Interface to StreamDeck.
     */
    void updateContextState(DcsInterface &dcs_interface, ESDConnectionManager *mConnectionManager);

    /**
     * @brief Updates settings from received json payload.
     * 
     * @param settings Json payload of settings values populated in Streamdeck Property Inspector.
     */
    void updateContextSettings(const json &settings);

private:
    using CompareConditionType = enum {
        EQUAL_TO = 0,
        LESS_THAN,
        GREATER_THAN
    };

    std::string context_; // Unique context ID used by Streamdeck to refer to instances of buttons.

    // Context settings.
    bool compare_monitor_is_set_ = false; // True if all DCS ID comparison monitor settings have been set.
    bool string_monitor_is_set_ = false;  // True if all DCS ID string monitor settings have been set.

    int dcs_id_compare_monitor_ = 0;
    CompareConditionType dcs_id_compare_condition_ = EQUAL_TO;
    float dcs_id_comparison_value_ = 0.0F;
    int dcs_id_string_monitor_ = 0;
};