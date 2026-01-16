add_library(SimulatorInterface STATIC)

target_sources(
    SimulatorInterface
    PRIVATE
        SimConnectionManager.cpp
        SimulatorInterface.cpp
        Protocols/DcsBiosProtocol.cpp
        Protocols/DcsBiosProtocol.h
        Protocols/DcsBiosStreamParser.cpp
        Protocols/DcsBiosStreamParser.h
        Protocols/DcsExportScriptProtocol.cpp
        Protocols/DcsExportScriptProtocol.h
    PUBLIC
        FILE_SET HEADERS
            BASE_DIRS
                .. # Include from backend-cpp/ root.
            FILES
                SimConnectionManager.h
                SimulatorInterface.h
                SimulatorInterfaceParameters.h
                SimulatorProtocolTypes.h
)

target_link_libraries(SimulatorInterface PUBLIC Utilities)

if(BUILD_TESTING)
    add_subdirectory(test)
    add_subdirectory(Protocols/test)
endif()
