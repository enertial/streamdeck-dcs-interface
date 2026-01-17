add_library(Utilities STATIC)

target_sources(
    Utilities
    PRIVATE
        Decimal.cpp
        JsonReader.cpp
        LuaReader.cpp
        StringUtilities.cpp
        UdpSocket.cpp
    PUBLIC
        FILE_SET HEADERS
            BASE_DIRS ${CMAKE_SOURCE_DIR}
            FILES
                Decimal.h
                JsonReader.h
                LuaReader.h
                StringUtilities.h
                UdpSocket.h
)

target_link_libraries(Utilities PUBLIC nlohmann_json PRIVATE lua-5.4.8)

if(BUILD_TESTING)
    add_subdirectory(test)
endif()
