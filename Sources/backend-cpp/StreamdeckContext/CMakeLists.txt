add_library(StreamdeckContext STATIC)

target_sources(StreamdeckContext
    PRIVATE
        BackwardsCompatibilityHandler.cpp
        StreamdeckContext.cpp

        ExportMonitors/ImageStateMonitor.cpp
        ExportMonitors/ImageStateMonitor.h
        ExportMonitors/IncrementMonitor.cpp
        ExportMonitors/IncrementMonitor.h
        ExportMonitors/TitleMonitor.cpp
        ExportMonitors/TitleMonitor.h

        SendActions/IncrementAction.cpp
        SendActions/IncrementAction.h
        SendActions/MomentaryAction.cpp
        SendActions/MomentaryAction.h
        SendActions/SendActionFactory.cpp
        SendActions/SendActionFactory.h
        SendActions/SendActionInterface.h
        SendActions/SwitchAction.cpp
        SendActions/SwitchAction.h

    PUBLIC FILE_SET HEADERS
        BASE_DIRS ..   # Include from backend-cpp/ root.
        FILES
            BackwardsCompatibilityHandler.h
            StreamdeckContext.h
)

target_link_libraries(StreamdeckContext 
    PUBLIC 
        ElgatoSD
        nlohmann_json
        SimulatorInterface
        Utilities
)

if(BUILD_TESTING)
    add_subdirectory(test)
    add_subdirectory(ExportMonitors/test)
    add_subdirectory(SendActions/test)
endif()

