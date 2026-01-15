find_package(GTest REQUIRED)
include(GoogleTest)

set(SOURCES
    BackwardsCompatibilityHandlerTest.cpp
    StreamdeckContextTest.cpp
)

add_executable(test_StreamdeckContext ${SOURCES})

# Link to the library being tested + GTest
target_link_libraries(test_StreamdeckContext PRIVATE 
    StreamdeckContext 
    GTest::gtest_main
)

gtest_discover_tests(test_StreamdeckContext)
