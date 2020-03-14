// Copyright 2020 Charles Tytler

#include "pch.h"

#include "StreamdeckContext.h"

#include "Common/EPLJSONUtils.h"
#include "Utilities.h"

StreamdeckContext::StreamdeckContext(const std::string &context)
{
    context_ = context;
}

StreamdeckContext::StreamdeckContext(const std::string &context, const json &settings)
{
    context_ = context;
    updateContextSettings(settings);
}

void StreamdeckContext::updateContextState(DcsInterface &dcs_interface, ESDConnectionManager *mConnectionManager)
{
    // Default button states:
    int state = 0;
    std::string title = "";

    if (compare_monitor_is_set_)
    {
        const std::string current_game_value_raw = dcs_interface.get_value_of_dcs_id(dcs_id_compare_monitor_);
        if (!current_game_value_raw.empty() && is_number(current_game_value_raw))
        {
            float current_game_value = std::stof(current_game_value_raw);
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

            state = set_context_state_to_second ? 1 : 0;
        }
    }

    if (string_monitor_is_set_)
    {
        const std::string current_game_string_value = dcs_interface.get_value_of_dcs_id(dcs_id_string_monitor_);
        if (!current_game_string_value.empty())
        {
            title = current_game_string_value;
        }
    }

    if (state != current_state_)
    {
        current_state_ = state;
        mConnectionManager->SetState(current_state_, context_);
    }
    if (title != current_title_)
    {
        current_title_ = title;
        mConnectionManager->SetTitle(current_title_, context_, kESDSDKTarget_HardwareAndSoftware);
    }
}

void StreamdeckContext::forceSendState(ESDConnectionManager *mConnectionManager)
{
    mConnectionManager->SetState(current_state_, context_);
}

void StreamdeckContext::updateContextSettings(const json &settings)
{
    //TODO: Allow comparsion of strings in addition to numeric values.

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
        dcs_id_comparison_value_ = std::stof(dcs_id_comparison_value_raw);
        if (dcs_id_compare_condition_raw == "EQUAL_TO")
        {
            dcs_id_compare_condition_ = EQUAL_TO;
        }
        else if (dcs_id_compare_condition_raw == "LESS_THAN")
        {
            dcs_id_compare_condition_ = LESS_THAN;
        }
        else // Default in Property Inspector html is GREATER_THAN.
        {
            dcs_id_compare_condition_ = GREATER_THAN;
        }
    }

    if (string_monitor_is_set_)
    {
        dcs_id_string_monitor_ = std::stoi(dcs_id_string_monitor_raw);
    }
}