file(GLOB LUA_SOURCES "src/*.c")

# IMPORTANT: Remove the standalone interpreter and compiler files
# These have their own 'main()' functions and will cause linker errors.
list(REMOVE_ITEM LUA_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/lua.c" 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/luac.c"
)

add_library(lua-5.4.8 STATIC ${LUA_SOURCES})

# Set include directory so others can find lua.h and lua.hpp
target_include_directories(lua-5.4.8 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

# MSVC specific security and compatibility fixes
# (Avoids warnings from using standard C functions as opposed to "secure" ones)
if(MSVC)
    target_compile_definitions(lua-5.4.8 PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
