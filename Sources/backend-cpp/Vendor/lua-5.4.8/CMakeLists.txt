add_library(lua-5.4.8 STATIC)
# set_target_properties(lua-5.4.8 PROPERTIES LINKER_LANGUAGE C)

target_sources(
    lua-5.4.8
    PUBLIC
        FILE_SET HEADERS
            BASE_DIRS
                src/ # Include lua.hpp directly
            FILES src/lua.hpp src/lua.h src/lualib.h src/lauxlib.h
)

target_sources(
    lua-5.4.8
    PRIVATE
        src/lapi.c
        src/lauxlib.c
        src/lbaselib.c
        src/lcode.c
        src/lcorolib.c
        src/lctype.c
        src/ldblib.c
        src/ldebug.c
        src/ldo.c
        src/ldump.c
        src/lfunc.c
        src/lgc.c
        src/linit.c
        src/liolib.c
        src/llex.c
        src/lmathlib.c
        src/lmem.c
        src/loadlib.c
        src/lobject.c
        src/lopcodes.c
        src/loslib.c
        src/lparser.c
        src/lstate.c
        src/lstring.c
        src/lstrlib.c
        src/ltable.c
        src/ltablib.c
        src/ltm.c
        src/lundump.c
        src/lutf8lib.c
        src/lvm.c
        src/lzio.c
        # Standalone files lua.c and luac.c are excluded as
        # they have their own 'main()' functions
        src/lapi.h
        src/lcode.h
        src/lctype.h
        src/ldebug.h
        src/ldo.h
        src/lfunc.h
        src/lgc.h
        src/ljumptab.h
        src/llex.h
        src/llimits.h
        src/lmem.h
        src/lobject.h
        src/lopcodes.h
        src/lopnames.h
        src/lparser.h
        src/lprefix.h
        src/lstate.h
        src/lstring.h
        src/ltable.h
        src/ltm.h
        src/luaconf.h
        src/lundump.h
        src/lvm.h
        src/lzio.h
)

# MSVC specific security and compatibility fixes
# (Avoids warnings from using standard C functions as opposed to "secure" ones)
if(MSVC)
    target_compile_definitions(lua-5.4.8 PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
