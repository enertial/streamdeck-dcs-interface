add_executable(SocketTester)

target_sources(SocketTester PRIVATE SocketTester.cpp)

target_link_libraries(SocketTester PRIVATE Utilities)

target_compile_definitions(SocketTester PRIVATE _CONSOLE)

# Output Directory
set_target_properties(
    SocketTester
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/SocketTester_bin"
)
