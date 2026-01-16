add_executable(ProtocolsTest)

target_sources(
    ProtocolsTest
    PRIVATE
        DcsBiosProtocolTest.cpp
        DcsBiosStreamParserTest.cpp
        DcsExportScriptProtocolTest.cpp
)

target_link_libraries(
    ProtocolsTest
    PRIVATE SimulatorInterface GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(ProtocolsTest)
