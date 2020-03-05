#include "gtest/gtest.h"

#include "../DcsInterface.h"
#include "../DcsInterface.cpp" // Included because I can't figure out Windows Linker

namespace test
{

const std::string kDcsListenerPort = "1901";   // Port number to receive DCS updates from.
const std::string kDcsSendPort = "1902";       // Port number which DCS commands will be sent to.
const std::string kDcsIpAddress = "127.0.0.1"; // IP Address on which to communicate with DCS -- Default LocalHost.

TEST(DcsInterfaceTest, unregister_monitor_while_empty)
{
    DcsInterface dcs_interface(kDcsListenerPort, kDcsSendPort, kDcsIpAddress);

    // Test that calling unregister with an empty registry does not cause issue.
    EXPECT_NO_THROW(dcs_interface.unregister_dcs_monitor("dummy_context"));
}

TEST(DcsInterfaceTest, registered_dcs_id_monitor)
{
    DcsInterface dcs_interface(kDcsListenerPort, kDcsSendPort, kDcsIpAddress);

    // Open a socket that will mock Send/Receive messages from DCS.
    DcsSocket mock_DCS(kDcsSendPort, kDcsListenerPort, kDcsIpAddress);

    // Register multiple (DCS ID : Context) pairs.
    dcs_interface.register_dcs_monitor(761, "context_for_761");
    dcs_interface.register_dcs_monitor(2026, "context_for_2026");
    dcs_interface.register_dcs_monitor(2027, "context_for_2027");
    dcs_interface.register_dcs_monitor(765, "context_for_765");

    // Send a single message from mock DCS that contains updates for all registered DCS ID's.
    const std::string mock_DCS_message = "header*761=1:765=2:2026=3:2027=4";
    mock_DCS.DcsSend(mock_DCS_message);
    std::vector<DcsIdValueUpdate> update = dcs_interface.get_next_dcs_update();

    // Expect that 4 total value updates were returned.
    EXPECT_EQ(update.size(), 4);

    // Expect that DCS ID value updates are received in the order they were sent.
    EXPECT_EQ(update[0].dcs_id, 761);
    EXPECT_EQ(update[0].dcs_id_value, "1");
    EXPECT_EQ(update[0].context, "context_for_761");

    EXPECT_EQ(update[1].dcs_id, 765);
    EXPECT_EQ(update[1].dcs_id_value, "2");
    EXPECT_EQ(update[1].context, "context_for_765");

    EXPECT_EQ(update[2].dcs_id, 2026);
    EXPECT_EQ(update[2].dcs_id_value, "3");
    EXPECT_EQ(update[2].context, "context_for_2026");

    EXPECT_EQ(update[3].dcs_id, 2027);
    EXPECT_EQ(update[3].dcs_id_value, "4");
    EXPECT_EQ(update[3].context, "context_for_2027");

    // Unregister some of the DCS ID's from being monitored.
    dcs_interface.unregister_dcs_monitor("context_for_2027");
    dcs_interface.unregister_dcs_monitor("context_for_761");

    // Send another message update with all DCS ID's still present.
    mock_DCS.DcsSend(mock_DCS_message);
    update = dcs_interface.get_next_dcs_update();

    // Expect that 2 total value updates were returned.
    EXPECT_EQ(update.size(), 2);

    // Expect that DCS ID value updates are received in the order they were sent.
    EXPECT_EQ(update[0].dcs_id, 765);
    EXPECT_EQ(update[0].dcs_id_value, "2");
    EXPECT_EQ(update[0].context, "context_for_765");

    EXPECT_EQ(update[1].dcs_id, 2026);
    EXPECT_EQ(update[1].dcs_id_value, "3");
    EXPECT_EQ(update[1].context, "context_for_2026");
}

TEST(DcsInterfaceTest, send_dcs_command)
{
    DcsInterface dcs_interface(kDcsListenerPort, kDcsSendPort, kDcsIpAddress);

    // Open a socket that will mock Send/Receive messages from DCS.
    DcsSocket mock_DCS(kDcsSendPort, kDcsListenerPort, kDcsIpAddress);

    const int button_id = 250;
    const int device_id = 24;
    const std::string value = "1";
    // Message is sent prepended with "C" prepended and 3000 added to Button ID.
    const std::string expected_msg_buffer = "C24,3250,1";
    dcs_interface.send_dcs_command(button_id, device_id, value);

    std::stringstream ss_received = mock_DCS.DcsReceive();
    EXPECT_EQ(ss_received.str(), expected_msg_buffer);
}

} // namespace test
