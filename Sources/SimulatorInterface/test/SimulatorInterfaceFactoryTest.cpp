// Copyright 2021 Charles Tytler

#include "gtest/gtest.h"

#include "SimulatorInterface/SimulatorInterfaceFactory.h"

namespace test
{

TEST(SimulatorInterfaceFactoryTest, UnknownBackend)
{
    const SimulatorConnectionSettings connection_settings = {"1908", "1909", "127.0.0.1", ""};
    auto ptr = SimulatorInterfaceFactory(connection_settings, "unknown_backend");
    EXPECT_FALSE(ptr);
}

TEST(SimulatorInterfaceFactoryTest, DcsBiosBackend)
{
    const SimulatorConnectionSettings connection_settings = {"1908", "1909", "127.0.0.1", ""};
    auto ptr = SimulatorInterfaceFactory(connection_settings, "DCS-BIOS");
    EXPECT_TRUE(ptr);
}

TEST(SimulatorInterfaceFactoryTest, DcsExportScriptBackend)
{
    const SimulatorConnectionSettings connection_settings = {"1908", "1909", "127.0.0.1", ""};
    auto ptr = SimulatorInterfaceFactory(connection_settings, "DCS-ExportScript");
    EXPECT_TRUE(ptr);
}

} // namespace test
