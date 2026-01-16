add_executable(SimulatorInterfaceTest)

target_sources(SimulatorInterfaceTest
    PRIVATE
        SimConnectionManagerTest.cpp
        SimulatorInterfaceTest.cpp
)

target_link_libraries(SimulatorInterfaceTest
    PRIVATE 
        SimulatorInterface
        GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(SimulatorInterfaceTest)

