// Copyright 2021 Charles Tytler

#include "gtest/gtest.h"

#include "StreamdeckContext/SendActions/SendActionFactory.h"

namespace test
{

TEST(SendActionFactoryTest, InvalidAction)
{
    const auto ptr = SendActionFactory().create("unknown_value");
    EXPECT_FALSE(ptr);
}
TEST(SendActionFactoryTest, ValidActions)
{
    const std::vector<std::string> valid_actions = {
        "com.ctytler.dcs.dcs-bios",
        "com.ctytler.dcs.lamp.button.two-state",
        "com.ctytler.dcs.static.button.one-state",
        "com.ctytler.dcs.static.text.one-state",
        "com.ctytler.dcs.increment.dial.two-state",
        "com.ctytler.dcs.increment.textdial.two-state",
        "com.ctytler.dcs.up-down.switch.two-state",
    };

    for (const auto &action : valid_actions) {
        const auto ptr = SendActionFactory().create(action);
        EXPECT_TRUE(ptr);
    }
}

} // namespace test
