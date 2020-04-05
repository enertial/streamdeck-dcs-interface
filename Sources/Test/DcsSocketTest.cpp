// Copyright 2020 Charles Tytler

#include "gtest/gtest.h"

#include "../DcsInterface/DcsSocket.cpp"

namespace test {

TEST(DcsSocketTest, empty_connection_settings) { EXPECT_THROW(DcsSocket dcs_socket("", "", ""), std::runtime_error); }

TEST(DcsSocketTest, invalid_connection_port_settings) {
    EXPECT_THROW(DcsSocket dcs_socket("19ab", "abc", "127.0.0.1"), std::runtime_error);
}

TEST(DcsSocketTest, invalid_connection_ip_addr_settings) {
    EXPECT_THROW(DcsSocket dcs_socket("1908", "1909", "127001"), std::runtime_error);
}

class DcsSocketTestFixture : public ::testing::Test {
  public:
    DcsSocketTestFixture()
        : sender_socket("1788", common_port, ip_address), receiver_socket(common_port, "1790", ip_address) {}

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
    EXPECT_THROW(DcsSocket duplicate_socket(common_port, "1801", ip_address), std::runtime_error);
}

TEST_F(DcsSocketTestFixture, receive_timeout) {
    std::stringstream ss_received = receiver_socket.DcsReceive();
    // Expect timeout after 100 msec and return of empty string.
    EXPECT_EQ(ss_received.str(), "");
}
} // namespace test
