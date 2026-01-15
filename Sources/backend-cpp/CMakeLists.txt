cmake_minimum_required(VERSION 3.15)
project(StreamdeckPluginBackend LANGUAGES CXX)

# TODO: Delete this if not desired
# Enforce Windows x64 MSVC
# if(NOT MSVC OR NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
#     message(FATAL_ERROR "This project requires MSVC x64.")
# endif()

# Global C++ Settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# TODO: Consider /W4 or /Wall
if(MSVC)
    add_compile_options(
        /W3              # Warning Level 3
        /WX              # Treat Warning As Error
        /sdl             # Enable Security Checks
        /MP              # Multi-processor compilation
        /Zc:__cplusplus  # Correct __cplusplus macro
    )
endif()

add_compile_definitions(
    UNICODE 
    _UNICODE
    _CONSOLE
)
# TODO: If you are using _CONSOLE, you might see a black command prompt
# window pop up when your plugin starts. If you eventually want your
# plugin to run silently in the background without that window,
# you would change your project type to a Windows Application and use
# _WINDOWS instead of _CONSOLE, but for development and debugging,
# using _CONSOLE is much easier because you can see your log output.


# Vendored dependencies
add_subdirectory(Vendor)

# Enable testing for the whole project (Gtest)
# enable_testing()

# Static Libraries
# add_subdirectory(Utilities)
add_subdirectory(ElgatoSD)
# add_subdirectory(SimulatorInterface)
# add_subdirectory(StreamdeckContext)

# Executable
# add_subdirectory(StreamdeckInterface)
