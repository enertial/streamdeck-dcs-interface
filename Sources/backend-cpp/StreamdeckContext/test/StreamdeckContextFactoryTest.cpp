// Copyright 2021 Charles Tytler

#include "gtest/gtest.h"

#include "StreamdeckContext/StreamdeckContextFactory.h"

namespace test
{

TEST(StreamdeckContextFactoryTest, UnknownActionUUID)
{
    auto factory = StreamdeckContextFactory();
    auto ptr = factory.create("unknown_value", "ctx", {});
    EXPECT_FALSE(ptr);
}
TEST(StreamdeckContextFactoryTest, MomentaryButtonWithLamp)
{
    auto factory = StreamdeckContextFactory();
    auto ptr = factory.create("com.ctytler.dcs.lamp.button.two-state", "ctx", {});
    EXPECT_TRUE(ptr);
}
TEST(StreamdeckContextFactoryTest, MomentaryButton)
{
    auto factory = StreamdeckContextFactory();
    auto ptr = factory.create("com.ctytler.dcs.static.button.one-state", "ctx", {});
    EXPECT_TRUE(ptr);
}
TEST(StreamdeckContextFactoryTest, StaticDisplay)
{
    auto factory = StreamdeckContextFactory();
    auto ptr = factory.create("com.ctytler.dcs.static.text.one-state", "ctx", {});
    EXPECT_TRUE(ptr);
}
TEST(StreamdeckContextFactoryTest, RotaryDial)
{
    auto factory = StreamdeckContextFactory();
    auto ptr = factory.create("com.ctytler.dcs.increment.dial.two-state", "ctx", {});
    EXPECT_TRUE(ptr);
}
TEST(StreamdeckContextFactoryTest, RotaryTextDial)
{
    auto factory = StreamdeckContextFactory();
    auto ptr = factory.create("com.ctytler.dcs.increment.textdial.two-state", "ctx", {});
    EXPECT_TRUE(ptr);
}
TEST(StreamdeckContextFactoryTest, SwitchInput)
{
    auto factory = StreamdeckContextFactory();
    auto ptr = factory.create("com.ctytler.dcs.up-down.switch.two-state", "ctx", {});
    EXPECT_TRUE(ptr);
}

} // namespace test
