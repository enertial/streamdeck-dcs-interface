// Copyright 2022 Charles Tytler

#include "gtest/gtest.h"

#include "StreamdeckInterface/BackwardsCompatibilityHandler.h"

namespace test
{

TEST(BackwardsCompatibilityHandlerTest, EmptyPayload) { EXPECT_EQ(json({}), backwardsCompatibilityHandler(json({}))); }

TEST(BackwardsCompatibilityHandlerTest, Version1_0Test)
{
    const json payload = {
        {"device_id", "device_id"},
        {"button_id", "button_id"},
    };
    const json expectedPayload = {
        {"device_id", "device_id"},
        {"button_id", "button_id"},
        {"send_address", "device_id,button_id"},
    };
    EXPECT_EQ(expectedPayload, backwardsCompatibilityHandler(payload));
}
