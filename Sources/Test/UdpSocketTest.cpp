// Copyright 2020 Charles Tytler

#include "gtest/gtest.h"

#include "../Utilities/UdpSocket.cpp"

namespace test
{
TEST(UdpSocketTest, empty_connection_settings) { EXPECT_THROW(UdpSocket dcs_socket("", "", ""), std::runtime_error); }

TEST(UdpSocketTest, invalid_connection_port_settings)
{
    EXPECT_THROW(UdpSocket dcs_socket("127.0.0.1", "19ab", "abc"), std::runtime_error);
}

TEST(UdpSocketTest, invalid_connection_ip_addr_settings)
{
    EXPECT_THROW(UdpSocket dcs_socket("127001", "1908", "1909"), std::runtime_error);
}

TEST(UdpSocketTest, invalid_multicast_ip_addr)
{
    EXPECT_THROW(UdpSocket second_receiver_socket("0.0.0.0", "5015", "7778", "23813222"), std::runtime_error);
}

TEST(UdpSocketTest, multicast_port_builds)
{
    // Test that no errors occur when connecting to multicast group.
    UdpSocket multicast_subscriber_socket("0.0.0.0", "5010", "7778", "239.255.50.10");
}

class UdpSocketTestFixture : public ::testing::Test
{
  public:
    UdpSocketTestFixture()
        : sender_socket(ip_address, "1788", common_port), receiver_socket(ip_address, common_port, "1790")
    {
    }

    UdpSocket sender_socket;
    UdpSocket receiver_socket;
    static inline std::string common_port = "1789";
    static inline std::string ip_address = "127.0.0.1";
};

TEST_F(UdpSocketTestFixture, send_and_receive)
{
    const std::string test_message = "test send from one UdpSocket to another.";
    sender_socket.send(test_message);
    std::stringstream ss_received = receiver_socket.receive();
    EXPECT_EQ(ss_received.str(), test_message);
}

TEST_F(UdpSocketTestFixture, receive_timeout)
{
    std::stringstream ss_received = receiver_socket.receive();
    // Expect timeout after 100 msec and return of empty string.
    EXPECT_EQ(ss_received.str(), "");
}

TEST_F(UdpSocketTestFixture, dynamic_tx_port_discovery)
{
    const std::string new_common_port = "1791";
    UdpSocket server_socket(ip_address, "1792", new_common_port);
    UdpSocket client_socket(ip_address, new_common_port);
    const std::string test_msg_a = "test_a";
    server_socket.send(test_msg_a);
    std::stringstream client_received_ss = client_socket.receive();
    EXPECT_EQ(test_msg_a, client_received_ss.str());
    // Expect client socket to have dynamically set tx_port to sender_socket's bound port.
    const std::string test_msg_b = "test_b";
    client_socket.send(test_msg_b);
    std::stringstream server_received_ss = server_socket.receive();
    EXPECT_EQ(test_msg_b, server_received_ss.str());
}
} // namespace test
