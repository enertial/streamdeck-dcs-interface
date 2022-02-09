// Copyright 2022 Charles Tytler

#include "gtest/gtest.h"

#include "StreamdeckContext/BackwardsCompatibilityHandler.h"

namespace test
{

TEST(BackwardsCompatibilityHandlerTest, EmptyPayload) { EXPECT_EQ(json({}), backwardsCompatibilityHandler(json({}))); }

TEST(BackwardsCompatibilityHandlerTest, VersionTest)
{
    json payload = {{"settings",
                     {
                         {"device_id", "device_id"},
                         {"button_id", "button_id"},
                     }}};
    json expectedPayload = {{"settings",
                             {
                                 {"device_id", "device_id"},
                                 {"button_id", "button_id"},
                                 {"send_address", "device_id,button_id"},
                             }}};

    EXPECT_EQ(expectedPayload.dump(), backwardsCompatibilityHandler(payload).dump());
}
} // namespace test
