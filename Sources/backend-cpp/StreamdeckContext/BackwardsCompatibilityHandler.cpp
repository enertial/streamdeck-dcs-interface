// Copyright 2022 Charles Tytler

#include "BackwardsCompatibilityHandler.h"

#include <iostream>
json backwardsCompatibilityHandler(const json &prevVersionPayload)
{
    json payload = prevVersionPayload;
    const auto prevSettings = prevVersionPayload["settings"];
    if (!prevSettings.contains("send_address") && prevSettings.contains("device_id") &&
        prevSettings.contains("button_id")) {
        const std::string device_id = prevSettings["device_id"];
        const std::string button_id = prevSettings["button_id"];
        payload["settings"]["send_address"] = device_id + "," + button_id;
    }
    return payload;
}
