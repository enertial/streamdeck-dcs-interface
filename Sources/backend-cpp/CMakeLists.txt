cmake_minimum_required(VERSION 3.23)

# Note: C is included because of the Vendor/lua-5.4.8 C library
project(StreamdeckPluginBackend LANGUAGES C CXX)

# Force the use of the Dynamic Runtime (/MD and /MDd) across the whole project
# This is added to align with the Vendored websocketpp library
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

# Global C++ Settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(
        /W3 # Warning Level 3
        /WX # Treat Warning As Error
        /sdl # Enable Security Checks
        /MP # Multi-processor compilation
        /Zc:__cplusplus # Correct __cplusplus macro
    )
endif()

add_compile_definitions(UNICODE _UNICODE _CONSOLE)
# TODO: If you are using _CONSOLE, you might see a black command prompt
# window pop up when your plugin starts. If you eventually want your
# plugin to run silently in the background without that window,
# you would change your project type to a Windows Application and use
# _WINDOWS instead of _CONSOLE, but for development and debugging,
# using _CONSOLE is much easier because you can see your log output.

# Testing
include(CTest)

if(BUILD_TESTING)
    include(FetchContent)

    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest
        GIT_TAG v1.17.0
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )

    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(googletest)
endif()

# Libraries
add_subdirectory(Vendor)
add_subdirectory(Utilities)
add_subdirectory(ElgatoSD)
add_subdirectory(SimulatorInterface)
add_subdirectory(StreamdeckContext)

# Executable
add_subdirectory(StreamdeckInterface)
