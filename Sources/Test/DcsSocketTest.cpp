#include "gtest/gtest.h"

#include "../DcsSocket.h"
#include "../DcsSocket.cpp" // Included because I can't figure out Windows Linker

namespace test
{
TEST(DcsSocketTest, send_and_receive)
{
	const std::string common_port = "1789";
	DcsSocket sender_socket("1788", common_port, "127.0.0.1");
	DcsSocket receiver_socket(common_port, "1790", "127.0.0.1");

	const std::string test_message = "test send from one DcsSocket to another.";

	sender_socket.DcsSend(test_message);
	std::stringstream ss_received = receiver_socket.DcsReceive();
	EXPECT_EQ(ss_received.str(), test_message);
}

TEST(DcsSocketTest, unavailable_port_bind)
{
	const std::string rx_port = "1801";
	const std::string tx_port = "1802";
	const std::string ip_address = "127.0.0.1";
	DcsSocket first_socket(rx_port, tx_port, ip_address);

	// Expect exception thrown if try to bind a new socket to same rx_port.
	EXPECT_ANY_THROW(DcsSocket duplicate_socket(rx_port, tx_port, ip_address));
}
} // namespace test