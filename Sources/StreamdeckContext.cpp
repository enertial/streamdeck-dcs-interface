// Copyright 2020 Charles Tytler

#include "pch.h"

#include "StreamdeckContext.h"

#include "Common/EPLJSONUtils.h"
#include "Utilities.h"

StreamdeckContext::StreamdeckContext(std::string context)
{
    context_ = context;
}

StreamdeckContext::StreamdeckContext(std::string context, json &settings)
{
    context_ = context;
    updateContextSettings(settings);
}

bool StreamdeckContext::operator==(const StreamdeckContext &rhs) const
{
    return (context_ == rhs.context_);
}

bool StreamdeckContext::operator<(const StreamdeckContext &rhs) const
{
    return (context_ < rhs.context_);
}

void StreamdeckContext::updateContextState(DcsInterface &dcs_interface, ESDConnectionManager *mConnectionManager)
{
    if (compare_monitor_is_set_)
    {
        float current_game_value = std::stof(dcs_interface.get_value_of_dcs_id(dcs_id_compare_monitor_));
        bool set_context_state_to_second = false;
        switch (dcs_id_compare_condition_)
        {
        case EQUAL_TO:
            set_context_state_to_second = (current_game_value == dcs_id_comparison_value_);
            break;
        case LESS_THAN:
            set_context_state_to_second = (current_game_value < dcs_id_comparison_value_);
            break;
        case GREATER_THAN:
            set_context_state_to_second = (current_game_value > dcs_id_comparison_value_);
            break;
        }

        if (set_context_state_to_second)
        {
            mConnectionManager->SetState(1, context_);
        }
        else
        {
            mConnectionManager->SetState(0, context_);
        }
    }

    if (string_monitor_is_set_)
    {
        mConnectionManager->SetTitle(dcs_interface.get_value_of_dcs_id(dcs_id_string_monitor_), context_, kESDSDKTarget_HardwareAndSoftware);
    }
}

void StreamdeckContext::updateContextSettings(json &settings)
{
    const std::string dcs_id_compare_monitor_raw = EPLJSONUtils::GetStringByName(settings, "dcs_id_compare_monitor");
    const std::string dcs_id_compare_condition_raw = EPLJSONUtils::GetStringByName(settings, "dcs_id_compare_condition");
    const std::string dcs_id_comparison_value_raw = EPLJSONUtils::GetStringByName(settings, "dcs_id_comparison_value");
    const std::string dcs_id_string_monitor_raw = EPLJSONUtils::GetStringByName(settings, "dcs_id_string_monitor");

    const bool compare_monitor_is_populated = (!dcs_id_compare_monitor_raw.empty() && is_integer(dcs_id_compare_monitor_raw));
    const bool comparison_value_is_populated = (!dcs_id_comparison_value_raw.empty() && is_number(dcs_id_comparison_value_raw));
    compare_monitor_is_set_ = compare_monitor_is_populated && comparison_value_is_populated;
    string_monitor_is_set_ = (!dcs_id_string_monitor_raw.empty() && is_integer(dcs_id_string_monitor_raw));

    if (compare_monitor_is_set_)
    {
        dcs_id_compare_monitor_ = std::stoi(dcs_id_compare_monitor_raw);
        dcs_id_comparison_value_ = std::stof(dcs_id_compare_monitor_raw);
        if (dcs_id_compare_condition_raw == "EQUAL_TO")
        {
            dcs_id_compare_condition_ = EQUAL_TO;
        }
        else if (dcs_id_compare_condition_raw == "LESS_THAN")
        {
            dcs_id_compare_condition_ = LESS_THAN;
        }
        else if (dcs_id_compare_condition_raw == "GREATER_THAN")
        {
            dcs_id_compare_condition_ = GREATER_THAN;
        }
    }

    if (string_monitor_is_set_)
    {
        dcs_id_string_monitor_ = std::stoi(dcs_id_compare_monitor_raw);
    }
}