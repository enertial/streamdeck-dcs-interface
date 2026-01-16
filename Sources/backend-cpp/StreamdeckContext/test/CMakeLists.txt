add_executable(StreamdeckContextTest)

target_sources(StreamdeckContextTest
    PRIVATE
        BackwardsCompatibilityHandlerTest.cpp
        StreamdeckContextTest.cpp
)

target_link_libraries(StreamdeckContextTest
    PRIVATE 
        StreamdeckContext
        MockESDConnectionManager
        GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(StreamdeckContextTest)

