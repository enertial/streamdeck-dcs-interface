add_executable(StreamdeckInterface)

target_sources(
    StreamdeckInterface
    PRIVATE main.cpp StreamdeckInterface.cpp
    PUBLIC
        FILE_SET HEADERS
            BASE_DIRS ${CMAKE_SOURCE_DIR}
            FILES StreamdeckInterface.h
)

target_link_libraries(
    StreamdeckInterface
    PRIVATE ElgatoSD SimulatorInterface StreamdeckContext Utilities
)
