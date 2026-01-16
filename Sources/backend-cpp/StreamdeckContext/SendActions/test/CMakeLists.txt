add_executable(SendActionsTest)

target_sources(SendActionsTest
    PRIVATE
        IncrementActionTest.cpp
        MomentaryActionTest.cpp
        SendActionFactoryTest.cpp
        SwitchActionTest.cpp
)

target_link_libraries(SendActionsTest
    PRIVATE 
        StreamdeckContext
        GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(SendActionsTest)

