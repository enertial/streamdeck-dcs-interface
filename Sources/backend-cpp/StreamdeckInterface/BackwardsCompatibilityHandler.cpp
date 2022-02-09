// Copyright 2022 Charles Tytler

#include "BackwardsCompatibilityHandler.h"

json backwardsCompatibilityHandler(json prevVersionPayload)
{
    json payload = prevVersionPayload;
    if (!prevVersionPayload.contains("send_address") && prevVersionPayload.contains("device_id") &&
        prevVersionPayload.contains("button_id")) {
        payload["send_address"] = payload["device_id"] + "," + payload["button_id"];
    }
    return payload;
}
