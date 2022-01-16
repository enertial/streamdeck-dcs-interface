// Copyright 2020 Charles Tytler

#include "StreamdeckContext.h"

#include "StreamdeckContext/SendActions/SendActionFactory.h"

#include "ElgatoSD/EPLJSONUtils.h"

StreamdeckContext::StreamdeckContext(const std::string &action, const std::string &context, const json &settings)
    : context_{context}, send_action_(SendActionFactory().create(action))
{
    updateContextSettings(settings);
}

bool StreamdeckContext::is_valid() { return (send_action_ != nullptr); }

void StreamdeckContext::updateContextState(SimulatorInterface *simulator_interface,
                                           ESDConnectionManager *mConnectionManager)
{

    const auto updated_state = comparison_monitor_.determineContextState(simulator_interface);
    const auto updated_title = title_monitor_.determineTitle(simulator_interface);

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
}

void StreamdeckContext::handleButtonPressedEvent(SimulatorInterface *simulator_interface,
                                                 ESDConnectionManager *mConnectionManager,
                                                 const json &inPayload)
{
    send_action_->handleButtonPressedEvent(simulator_interface, mConnectionManager, inPayload);
}

void StreamdeckContext::handleButtonReleasedEvent(SimulatorInterface *simulator_interface,
                                                  ESDConnectionManager *mConnectionManager,
                                                  const json &inPayload)
{
    send_action_->handleButtonReleasedEvent(simulator_interface, mConnectionManager, inPayload);

    // The Streamdeck will by default change a context's state after a KeyUp event, so a force send of the current
    // context's state will keep the button state in sync with the plugin.
    if (send_action_->delay_send_state()) {
        forceSendStateAfterDelay(NUM_FRAMES_DELAY_FORCED_STATE_UPDATE);
    } else {
        forceSendState(mConnectionManager);
    }
}
