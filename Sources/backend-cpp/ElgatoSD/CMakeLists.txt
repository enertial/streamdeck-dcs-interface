set(SOURCES
    ESDConnectionManager.cpp
    ESDLocalizer.cpp
    ESDUtilitiesWindows.cpp
    pch.cpp
)

set(HEADERS
    EPLJSONUtils.h
    ESDBasePlugin.h
    ESDConnectionManager.h
    ESDLocalizer.h
    ESDSDKDefines.h
    ESDUtilities.h
    pch.h
)

# TODO: Consider deleting the pch.* files, as they are not used as precompiled headers

add_library(ElgatoSD STATIC ${SOURCES} ${HEADERS})

target_link_libraries(ElgatoSD 
    PRIVATE 
        nlohmann_json
        websocketpp
)

