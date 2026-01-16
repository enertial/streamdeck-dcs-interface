add_executable(StreamdeckInterface)

target_sources(
    StreamdeckInterface
    PRIVATE main.cpp StreamdeckInterface.cpp
    PUBLIC
        FILE_SET HEADERS
            BASE_DIRS
                .. # Include from backend-cpp/ root.
            FILES StreamdeckInterface.h
)

target_link_libraries(
    StreamdeckInterface
    PRIVATE ElgatoSD SimulatorInterface StreamdeckContext Utilities
)
