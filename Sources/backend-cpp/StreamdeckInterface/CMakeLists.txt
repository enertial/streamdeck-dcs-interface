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

set_target_properties(
    StreamdeckInterface
    PROPERTIES INTERPROCEDURAL_OPTIMIZATION_RELEASE ON
)

set_target_properties(
    StreamdeckInterface
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

include(InstallRequiredSystemLibraries)

if(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)
    add_custom_command(
        TARGET StreamdeckInterface
        POST_BUILD
        COMMAND
            ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
            "$<TARGET_FILE_DIR:StreamdeckInterface>"
        COMMENT "Copying system runtime DLLs to output directory..."
    )
endif()
