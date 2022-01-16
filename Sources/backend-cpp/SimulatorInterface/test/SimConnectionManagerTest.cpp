// Copyright 2021 Charles Tytler

#include "gtest/gtest.h"

#include "SimulatorInterface/SimConnectionManager.h"

#include "SimulatorInterface/SimulatorInterfaceParameters.h"

namespace test
{

TEST(SimConnectionManagerTest, InitializeAsNotConnected)
{
    auto mgr = SimConnectionManager();
    EXPECT_FALSE(mgr.is_connected(Protocol::DCS_BIOS));
    EXPECT_FALSE(mgr.is_connected(Protocol::DCS_ExportScript));

    const auto settings = SimulatorConnectionSettings{"0", "0", "0", "0"};
    EXPECT_FALSE(mgr.is_connected_with_settings(Protocol::DCS_BIOS, settings));
    EXPECT_FALSE(mgr.is_connected_with_settings(Protocol::DCS_ExportScript, settings));
}

TEST(SimConnectionManagerTest, ConnectToMultipleProtocols)
{
    // Connect to one protocol.
    const auto dcs_bios_settings = SimulatorConnectionSettings{"1910", "1920", "127.0.0.1", ""};
    auto mgr = SimConnectionManager();
    mgr.connect_to_protocol(Protocol::DCS_BIOS, dcs_bios_settings);
    EXPECT_TRUE(mgr.is_connected(Protocol::DCS_BIOS));
    // Still expect other protocol(s) to not be connected.
    EXPECT_FALSE(mgr.is_connected(Protocol::DCS_ExportScript));

    // Connect to a second protocol.
    const auto dcs_exportscript_settings = SimulatorConnectionSettings{"1810", "1820", "127.0.0.1", ""};
    mgr.connect_to_protocol(Protocol::DCS_ExportScript, dcs_exportscript_settings);
    EXPECT_TRUE(mgr.is_connected(Protocol::DCS_BIOS));
    EXPECT_TRUE(mgr.is_connected(Protocol::DCS_ExportScript));

    // Check matching settings.
    EXPECT_TRUE(mgr.is_connected_with_settings(Protocol::DCS_BIOS, dcs_bios_settings));
    EXPECT_TRUE(mgr.is_connected_with_settings(Protocol::DCS_ExportScript, dcs_exportscript_settings));
    // Check mismatched settings returns false.
    EXPECT_FALSE(mgr.is_connected_with_settings(Protocol::DCS_BIOS, dcs_exportscript_settings));
    EXPECT_FALSE(mgr.is_connected_with_settings(Protocol::DCS_ExportScript, dcs_bios_settings));
}

TEST(SimConnectionManagerTest, ConnectToInvalidProtocol)
{
    const auto invalid_settings = SimulatorConnectionSettings{"0", "0", "0", "0"};
    auto mgr = SimConnectionManager();
    // Expect exception thrown within UDPSocket to elevate up.
    EXPECT_THROW(mgr.connect_to_protocol(Protocol::DCS_BIOS, invalid_settings), std::runtime_error);
}

TEST(SimConnectionManagerTest, DisconnectProtocol)
{
    const auto dcs_bios_settings = SimulatorConnectionSettings{"1910", "1920", "127.0.0.1", ""};
    auto mgr = SimConnectionManager();
    mgr.connect_to_protocol(Protocol::DCS_BIOS, dcs_bios_settings);
    EXPECT_TRUE(mgr.is_connected(Protocol::DCS_BIOS));

    // Disconnect.
    mgr.disconnect_protocol(Protocol::DCS_BIOS);
    EXPECT_FALSE(mgr.is_connected(Protocol::DCS_BIOS));
}

TEST(SimConnectionManagerTest, GetInterfaceAndSend)
{
    // Create connection.
    const auto settings = SimulatorConnectionSettings{"1910", "1920", "127.0.0.1", ""};
    auto mgr = SimConnectionManager();
    mgr.connect_to_protocol(Protocol::DCS_BIOS, settings);
    // Create listener socket.
    UdpSocket listener_socket(settings.ip_address, settings.tx_port, settings.rx_port, settings.multicast_address);

    mgr.get_interface(Protocol::DCS_BIOS)->send_command("TEST_SEND", "VALUE");
    std::stringstream ss_received = listener_socket.receive_stream();
    EXPECT_EQ(ss_received.str(), "TEST_SEND VALUE\n");
}

TEST(SimConnectionManagerTest, UpdateAll)
{
    const auto LOCAL_HOST = "127.0.0.1";
    const auto BIOS_PORT = "1908";
    const auto EXPSCRIPT_PORT = "1909";
    const auto UNUSED = "1910";

    const auto dcs_bios_settings = SimulatorConnectionSettings{BIOS_PORT, UNUSED, LOCAL_HOST, ""};
    const auto dcs_exportscript_settings = SimulatorConnectionSettings{EXPSCRIPT_PORT, UNUSED, LOCAL_HOST, ""};
    UdpSocket mock_dcs_bios(LOCAL_HOST, UNUSED, BIOS_PORT);
    UdpSocket mock_dcs_exportscript(LOCAL_HOST, UNUSED, EXPSCRIPT_PORT);

    // Set up protocols.
    auto mgr = SimConnectionManager();
    mgr.connect_to_protocol(Protocol::DCS_BIOS, dcs_bios_settings);
    mgr.connect_to_protocol(Protocol::DCS_ExportScript, dcs_exportscript_settings);

    // Send DCS-BIOS protocol update.
    // clang-format off
    const auto msg_size = 16;
    const char mock_dcs_bios_message[msg_size] = {0x55, 0x55, 0x55, 0x55,                   // Sync frame
                                           0x78, 0x56, 0x02, 0x00, 0x07, 0x00,              // Addr 0x5678 (2 bytes)
                                           (char)0xFE, (char)0xFF, 0x02, 0x00, 0x00, 0x00}; // End of frame
    // clang-format on
    mock_dcs_bios.send_bytes(mock_dcs_bios_message, msg_size);

    // Send DCS-ExportScript protocol update.
    mock_dcs_exportscript.send_string("header*5679=1");

    mgr.update_all();
    EXPECT_TRUE(mgr.get_interface(Protocol::DCS_BIOS)->get_value_at_addr(SimulatorAddress(0x5678, 0xFFFF, 0)));
    EXPECT_TRUE(mgr.get_interface(Protocol::DCS_ExportScript)->get_value_at_addr(SimulatorAddress(5679)));
}

} // namespace test
