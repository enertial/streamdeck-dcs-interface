// Copyright 2022 Charles Tytler

#pragma once

#include "StreamdeckContext/SendActions/SendActionInterface.h"

#include <memory>
#include <unordered_map>

class SendActionFactory
{
  public:
    SendActionFactory();

    /**
     * @brief Factory method that generates action-specific instances of SendAction.
     *
     * @param action_uuid The identifier streamdeck uses for individual button types.
     * @return Interface to the created Send Action or NULL pointer if unable to create.
     */
    std::unique_ptr<SendActionInterface> create(const std::string &action_uuid);

  private:
    enum class ButtonAction { UNDEFINED, MOMENTARY, INCREMENT, SWITCH };
    std::unordered_map<std::string, ButtonAction> button_action_from_uuid_;
};
