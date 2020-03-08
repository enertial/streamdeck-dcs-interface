#include "gtest/gtest.h"

#include "../DcsInterface.h"
#include "../DcsInterface.cpp" // Included because I can't figure out Windows Linker

namespace test
{

const std::string kDcsListenerPort = "1901";   // Port number to receive DCS updates from.
const std::string kDcsSendPort = "1902";       // Port number which DCS commands will be sent to.
const std::string kDcsIpAddress = "127.0.0.1"; // IP Address on which to communicate with DCS -- Default LocalHost.

TEST(DcsInterfaceTest, registered_dcs_id_monitor)
{
    DcsInterface dcs_interface(kDcsListenerPort, kDcsSendPort, kDcsIpAddress);

    // Open a socket that will mock Send/Receive messages from DCS.
    DcsSocket mock_DCS(kDcsSendPort, kDcsListenerPort, kDcsIpAddress);

    // Send a single message from mock DCS that contains updates for all registered DCS ID's.
    const std::string mock_DCS_message = "header*761=1:765=2:2026=3:2027=4";
    mock_DCS.DcsSend(mock_DCS_message);

    // TODO: Query for ID updates.
    EXPECT_TRUE(true);
}

TEST(DcsInterfaceTest, send_dcs_command)
{
    DcsInterface dcs_interface(kDcsListenerPort, kDcsSendPort, kDcsIpAddress);

    // Open a socket that will mock Send/Receive messages from DCS.
    DcsSocket mock_DCS(kDcsSendPort, kDcsListenerPort, kDcsIpAddress);

    const int button_id = 250;
    const std::string device_id = "24";
    const std::string value = "1";
    // Message is sent prepended with "C" prepended and 3000 added to Button ID.
    const std::string expected_msg_buffer = "C24,3250,1";
    dcs_interface.send_dcs_command(button_id, device_id, value);

    std::stringstream ss_received = mock_DCS.DcsReceive();
    EXPECT_EQ(ss_received.str(), expected_msg_buffer);
}

} // namespace test
