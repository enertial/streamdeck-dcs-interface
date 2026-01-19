// Copyright 2022 Charles Tytler

#include "SendActionFactory.h"

#include "StreamdeckContext/SendActions/IncrementAction.h"
#include "StreamdeckContext/SendActions/MomentaryAction.h"
#include "StreamdeckContext/SendActions/SwitchAction.h"

SendActionFactory::SendActionFactory()
{
    // Register UUID string values to corresponding Button Action types.
    button_action_from_uuid_["com.ctytler.dcs.dcs-bios"] = ButtonAction::MOMENTARY;
    button_action_from_uuid_["com.ctytler.dcs.lamp.button.two-state"] = ButtonAction::MOMENTARY;
    button_action_from_uuid_["com.ctytler.dcs.static.button.one-state"] = ButtonAction::MOMENTARY;
    button_action_from_uuid_["com.ctytler.dcs.static.text.one-state"] = ButtonAction::MOMENTARY;
    button_action_from_uuid_["com.ctytler.dcs.increment.dial.two-state"] = ButtonAction::INCREMENT;
    button_action_from_uuid_["com.ctytler.dcs.increment.textdial.two-state"] = ButtonAction::INCREMENT;
    button_action_from_uuid_["com.ctytler.dcs.encoder.rotary"] = ButtonAction::INCREMENT;
    button_action_from_uuid_["com.ctytler.dcs.encoder.rotary.text"] = ButtonAction::INCREMENT;
    button_action_from_uuid_["com.ctytler.dcs.up-down.switch.two-state"] = ButtonAction::SWITCH;
}

std::unique_ptr<SendActionInterface> SendActionFactory::create(const std::string &action_uuid)
{
    switch (button_action_from_uuid_[action_uuid]) {
    case ButtonAction::MOMENTARY:
        return std::make_unique<MomentaryAction>();
    case ButtonAction::INCREMENT:
        return std::make_unique<IncrementAction>();
    case ButtonAction::SWITCH:
        return std::make_unique<SwitchAction>();
    default:
        return nullptr;
    }
}
