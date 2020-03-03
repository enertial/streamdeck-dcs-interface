#include "gtest/gtest.h"

#include "../DcsInterface.h"
#include "../DcsInterface.cpp" // Included because I can't figure out Windows Linker

namespace test
{

TEST(DcsInterfaceTest, TestName)
{
    constexpr int kDcsListenerSocket = 1625;
    constexpr int kDcsSendSocket = 26027;
    DcsInterface dcs_interface(kDcsListenerSocket, kDcsSendSocket);

    // Test that calling unregister with an unregistred string does not cause issue.
    dcs_interface.unregister_dcs_monitor("dummy");

    dcs_interface.register_dcs_monitor(761, "12345");
    dcs_interface.register_dcs_monitor(2026, "22345");
    dcs_interface.register_dcs_monitor(2027, "32325");
    dcs_interface.register_dcs_monitor(2028, "72325");
    dcs_interface.register_dcs_monitor(765, "42345");
    dcs_interface.unregister_dcs_monitor("32325");

    // Test that data with the monitored DCS ID comes through.
    for (int i = 0; i < 300; ++i)
    {
        std::vector<DcsIdValueUpdate> output = dcs_interface.get_next_dcs_update();

        for (const auto &update : output)
        {
            std::cout << "Received update - DCS ID " << update.dcs_id
                      << " = " << update.dcs_id_value
                      << "  [Context " << update.context << "]" << std::endl;
        }
    }

    EXPECT_TRUE(true);
}

} // namespace test