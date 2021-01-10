// Copyright 2020 Charles Tytler

#include "gtest/gtest.h"

#include "../DcsInterface/Utilities/DcsSocket.cpp"

namespace test {

TEST(DcsSocketTest, empty_connection_settings) { EXPECT_THROW(DcsSocket dcs_socket("", "", ""), std::runtime_error); }

TEST(DcsSocketTest, invalid_connection_port_settings) {
    EXPECT_THROW(DcsSocket dcs_socket("127.0.0.1", "19ab", "abc"), std::runtime_error);
}

TEST(DcsSocketTest, invalid_connection_ip_addr_settings) {
    EXPECT_THROW(DcsSocket dcs_socket("127001", "1908", "1909"), std::runtime_error);
}

class DcsSocketTestFixture : public ::testing::Test {
  public:
    DcsSocketTestFixture()
        : sender_socket(ip_address, "1788", common_port), receiver_socket(ip_address, common_port, "1790") {}

    DcsSocket sender_socket;
    DcsSocket receiver_socket;
    static inline std::string common_port = "1789";
    static inline std::string ip_address = "127.0.0.1";
};

TEST_F(DcsSocketTestFixture, send_and_receive) {
    const std::string test_message = "test send from one DcsSocket to another.";
    sender_socket.DcsSend(test_message);
    std::stringstream ss_received = receiver_socket.DcsReceive();
    EXPECT_EQ(ss_received.str(), test_message);
}

TEST_F(DcsSocketTestFixture, unavailable_port_bind) {
    // Expect exception thrown if try to bind a new socket to same rx_port.
    EXPECT_THROW(DcsSocket duplicate_socket(ip_address, common_port, "1801"), std::runtime_error);
}

TEST_F(DcsSocketTestFixture, receive_timeout) {
    std::stringstream ss_received = receiver_socket.DcsReceive();
    // Expect timeout after 100 msec and return of empty string.
    EXPECT_EQ(ss_received.str(), "");
}

TEST_F(DcsSocketTestFixture, dynamic_tx_port_discovery) {
    const std::string new_common_port = "1791";
    DcsSocket server_socket(ip_address, "1792", new_common_port);
    DcsSocket client_socket(ip_address, new_common_port);
    const std::string test_msg_a = "test_a";
    server_socket.DcsSend(test_msg_a);
    std::stringstream client_received_ss = client_socket.DcsReceive();
    EXPECT_EQ(test_msg_a, client_received_ss.str());
    // Expect client socket to have dynamically set tx_port to sender_socket's bound port.
    const std::string test_msg_b = "test_b";
    client_socket.DcsSend(test_msg_b);
    std::stringstream server_received_ss = server_socket.DcsReceive();
    EXPECT_EQ(test_msg_b, server_received_ss.str());
}
} // namespace test
