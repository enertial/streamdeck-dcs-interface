// Copyright 2021 Charles Tytler

#include "pch.h"

#include "StreamdeckContextFactory.h"

#include "SendActions/IncrementContext.h"
#include "SendActions/MomentaryContext.h"
#include "SendActions/SwitchContext.h"

StreamdeckContextFactory::StreamdeckContextFactory()
{
    // Register UUID string values to corresponding Button Action types.
    std::unordered_map<std::string, ButtonAction> button_action_from_uuid_;
    button_action_from_uuid_["com.ctytler.dcs.lamp.button.two-state"] = ButtonAction::MOMENTARY;
    button_action_from_uuid_["com.ctytler.dcs.static.button.one-state"] = ButtonAction::MOMENTARY;
    button_action_from_uuid_["com.ctytler.dcs.static.text.one-state"] = ButtonAction::MOMENTARY;
    button_action_from_uuid_["com.ctytler.dcs.increment.dial.two-state"] = ButtonAction::INCREMENT;
    button_action_from_uuid_["com.ctytler.dcs.increment.textdial.two-state"] = ButtonAction::INCREMENT;
    button_action_from_uuid_["com.ctytler.dcs.up-down.switch.two-state"] = ButtonAction::SWITCH;
}

std::unique_ptr<StreamdeckContext>
StreamdeckContextFactory::create(const std::string &action_uuid, const std::string &context, const json &settings)
{
    switch (button_action_from_uuid_[action_uuid]) {
    case ButtonAction::MOMENTARY:
        return std::unique_ptr<StreamdeckContext>(new MomentaryContext(context, settings));
    case ButtonAction::INCREMENT:
        return std::unique_ptr<StreamdeckContext>(new IncrementContext(context, settings));
    case ButtonAction::SWITCH:
        return std::unique_ptr<StreamdeckContext>(new SwitchContext(context, settings));
    default:
        return NULL;
    }
}
