add_library(SimulatorInterface STATIC)

target_sources(SimulatorInterface
    PRIVATE
        SimConnectionManager.cpp
        SimulatorInterface.cpp
    PUBLIC FILE_SET HEADERS
        BASE_DIRS ..   # Include from backend-cpp/ root.
        FILES
            SimConnectionManager.h
            SimulatorInterface.h
            SimulatorInterfaceParameters.h
            SimulatorProtocolTypes.h
)

target_link_libraries(SimulatorInterface 
    PUBLIC 
        Utilities
)

