add_library(MockESDConnectionManager INTERFACE)

target_sources(MockESDConnectionManager INTERFACE MockESDConnectionManager.h)

target_include_directories(
    MockESDConnectionManager
    INTERFACE ${CMAKE_SOURCE_DIR}
)
