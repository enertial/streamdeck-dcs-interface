// Copyright 2020 Charles Tytler

#include "StreamdeckContext.h"

#include "ElgatoSD/EPLJSONUtils.h"

StreamdeckContext::StreamdeckContext(const std::string &context) : context_{context} {};

StreamdeckContext::StreamdeckContext(const std::string &context, const json &settings) : context_{context}
{
    updateContextSettings(settings);
}

void StreamdeckContext::updateContextState(BaseSimulatorInterface &dcs_interface,
                                           ESDConnectionManager *mConnectionManager)
{

    const auto updated_state = comparison_monitor_.determineContextState(dcs_interface);
    const auto updated_title = title_monitor_.determineTitle(dcs_interface);
    increment_monitor_.update(dcs_interface);

    if (updated_state != current_state_) {
        current_state_ = updated_state;
        mConnectionManager->SetState(current_state_, context_);
    }
    if (updated_title != current_title_) {
        current_title_ = updated_title;
        mConnectionManager->SetTitle(current_title_, context_, kESDSDKTarget_HardwareAndSoftware);
    }

    if (delay_for_force_send_state_) {
        if (delay_for_force_send_state_.value()-- <= 0) {
            mConnectionManager->SetState(current_state_, context_);
            delay_for_force_send_state_.reset();
        }
    }
}

void StreamdeckContext::forceSendState(ESDConnectionManager *mConnectionManager)
{
    mConnectionManager->SetState(current_state_, context_);
}

void StreamdeckContext::forceSendStateAfterDelay(const int delay_count)
{
    delay_for_force_send_state_.emplace(delay_count);
}

void StreamdeckContext::updateContextSettings(const json &settings)
{
    comparison_monitor_.update_settings(settings);
    title_monitor_.update_settings(settings);
    increment_monitor_.update_settings(settings);
}
