#include "gtest/gtest.h"

#include "../DcsSocket.h"
#include "../DcsSocket.cpp" // Included because I can't figure out Windows Linker

namespace test
{
TEST(DcsSocketTest, print_out_some_data)
{
	// This is not a true test, but allows visibility that the socket connection is working.
	// Requires that something (DCS) is actively publishing to the specified port.
	const std::string kDcsListenerSocket = "1625";
	const std::string kDcsSendSocket = "26027";
	const std::string kDcsSendIpAddress = "127.0.0.1";
	DcsSocket dcs_socket(kDcsListenerSocket, kDcsSendSocket, kDcsSendIpAddress);

	// Listen to 10 messages.
	for (int i = 0; i < 10; ++i)
	{
		std::cout << "Messdage: " << std::endl;
		std::stringstream ss_out = dcs_socket.DcsReceive();
		std::cout << ss_out.str();
	}
	EXPECT_TRUE(true);
}
} // namespace test