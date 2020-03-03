#include "gtest/gtest.h"

#include "../DcsSocket.h"
#include "../DcsSocket.cpp" // Included because I can't figure out Windows Linker

namespace test
{
TEST(DcsSocketTest, print_out_some_data)
{
	// This is not a true test, but allows visibility that the socket connection is working.
	// Requires that something (DCS) is actively publishing to the specified port.
	constexpr int kDcsListenerSocket = 1625;
	constexpr int kDcsSendSocket = 26027;
	DcsSocket dcs_socket(kDcsListenerSocket, kDcsSendSocket);

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