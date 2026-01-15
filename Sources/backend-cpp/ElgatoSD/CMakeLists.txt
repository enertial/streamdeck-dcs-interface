add_library(ElgatoSD STATIC)

target_sources(ElgatoSD
    PRIVATE
        ESDConnectionManager.cpp
        ESDLocalizer.cpp
        ESDUtilitiesWindows.cpp
        pch.cpp

    PUBLIC FILE_SET HEADERS
        BASE_DIRS ..   # Include from backend-cpp/ root.
        FILES
            EPLJSONUtils.h
            ESDBasePlugin.h
            ESDConnectionManager.h
            ESDLocalizer.h
            ESDSDKDefines.h
            ESDUtilities.h
            pch.h
# TODO: Consider deleting the pch.* files, as they are not used as precompiled headers
)

target_link_libraries(ElgatoSD 
    PUBLIC 
        nlohmann_json
        websocketpp
)

