// Copyright 2021 Charles Tytler

#pragma once

#include "StreamdeckContext/StreamdeckContext.h"

#include <memory>
#include <unordered_map>

class StreamdeckContextFactory
{
  public:
    StreamdeckContextFactory();

    /**
     * @brief Factory method that generates action-specific instances of StreamdeckContext.
     *
     * @param action_uuid The identifier streamdeck uses for individual button types.
     * @param context Unique context ID used by Streamdeck to refer to instances of buttons.
     * @param settings Json payload of settings values populated in Streamdeck Property Inspector.
     */
    std::unique_ptr<StreamdeckContext>
    create(const std::string &action_uuid, const std::string &context, const json &settings);

  private:
    enum class ButtonAction { UNDEFINED, MOMENTARY, INCREMENT, SWITCH };
    std::unordered_map<std::string, ButtonAction> button_action_from_uuid_;
};