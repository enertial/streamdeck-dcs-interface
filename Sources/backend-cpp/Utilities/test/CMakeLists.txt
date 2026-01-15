add_executable(UtilitiesTest)

target_sources(UtilitiesTest
    PRIVATE
        DecimalTest.cpp
        JsonReaderTest.cpp
        LuaReaderTest.cpp
        StringUtilitiesTest.cpp
        UdpSocketTest.cpp
        sample.json
)

target_link_libraries(UtilitiesTest
    PRIVATE 
        Utilities
        GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(UtilitiesTest
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../
)

