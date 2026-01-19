// Copyright 2020 Charles Tytler

#include "StreamdeckContext.h"

#include "StreamdeckContext/SendActions/IncrementAction.h"
#include "StreamdeckContext/SendActions/SendActionFactory.h"

#include "ElgatoSD/EPLJSONUtils.h"

StreamdeckContext::StreamdeckContext(const std::string &action, const std::string &context, const json &settings)
    : context_{context}, send_action_(SendActionFactory().create(action))
{
    protocol_ = (action == "com.ctytler.dcs.dcs-bios") ? Protocol::DCS_BIOS : Protocol::DCS_ExportScript;
    updateContextSettings(settings);
}

bool StreamdeckContext::is_valid() { return (send_action_ != nullptr); }

Protocol StreamdeckContext::protocol() { return protocol_; }

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

    // Update encoder display with current increment value if this is an encoder action
    if (send_action_) {
        auto *increment_action = dynamic_cast<IncrementAction *>(send_action_.get());
        if (increment_action) {
            const std::string current_value = increment_action->getCurrentDisplayValue(simulator_interface, settings_);
            
            if (!current_value.empty()) {
                json feedback;
                feedback["value"] = current_value;
                
                // Calculate indicator (gauge) based on min/max/current
                const auto min_str = EPLJSONUtils::GetStringByName(settings_, "increment_min");
                const auto max_str = EPLJSONUtils::GetStringByName(settings_, "increment_max");
                const auto dcs_id_str = EPLJSONUtils::GetStringByName(settings_, "dcs_id_increment_monitor");
                
                if (!min_str.empty() && !max_str.empty() && is_integer(dcs_id_str)) {
                    try {
                        double min_val = std::stod(min_str);
                        double max_val = std::stod(max_str);
                        int dcs_id = std::stoi(dcs_id_str);
                        
                        auto maybe_current = simulator_interface->get_value_at_addr(dcs_id);
                        if (maybe_current.has_value()) {
                            double current = std::stod(maybe_current.value().str());
                            
                            // Calculate percentage (0-100)
                            double range = max_val - min_val;
                            if (range > 0) {
                                double percentage = ((current - min_val) / range) * 100.0;
                                // Clamp to 0-100
                                if (percentage < 0.0) percentage = 0.0;
                                if (percentage > 100.0) percentage = 100.0;
                                feedback["indicator"] = static_cast<int>(percentage);
                            }
                        }
                    } catch (...) {
                        // Ignore errors in indicator calculation
                    }
                }
                
                // Send feedback on every update, not just when value changes
                if (current_value != last_encoder_display_value_) {
                    last_encoder_display_value_ = current_value;
                }
                mConnectionManager->SetFeedback(feedback, context_);
            }
        }
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
    settings_ = settings;
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

void StreamdeckContext::handleEncoderRotation(SimulatorInterface *simulator_interface,
                                             ESDConnectionManager *mConnectionManager,
                                             const json &inPayload,
                                             int ticks)
{
    // Cast to IncrementAction to access encoder-specific methods
    auto *increment_action = dynamic_cast<IncrementAction *>(send_action_.get());
    if (increment_action) {
        // Create a new payload with stored settings
        json payload_with_settings = inPayload;
        payload_with_settings["settings"] = settings_;
        
        increment_action->handleEncoderRotation(simulator_interface, mConnectionManager, payload_with_settings, ticks);
    }
}

void StreamdeckContext::handleEncoderPress(SimulatorInterface *simulator_interface,
                                          ESDConnectionManager *mConnectionManager,
                                          const json &inPayload)
{
    // Cast to IncrementAction to access encoder-specific methods
    auto *increment_action = dynamic_cast<IncrementAction *>(send_action_.get());
    if (increment_action) {
        // Create a new payload with stored settings
        json payload_with_settings = inPayload;
        payload_with_settings["settings"] = settings_;
        
        increment_action->handleEncoderPress(simulator_interface, mConnectionManager, payload_with_settings);
    }
    
    // Force send state update after encoder press
    forceSendState(mConnectionManager);
}
