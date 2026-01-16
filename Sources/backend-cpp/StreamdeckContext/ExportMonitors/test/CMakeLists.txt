add_executable(ExportMonitorsTest)

target_sources(ExportMonitorsTest
    PRIVATE
        ImageStateMonitorTest.cpp
        IncrementMonitorTest.cpp
        TitleMonitorTest.cpp
)

target_link_libraries(ExportMonitorsTest
    PRIVATE 
        StreamdeckContext
        GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(ExportMonitorsTest)

