// Copyright 2020 Charles Tytler

#include "pch.h"

#include "DcsIdLookup.h"

#include "../Vendor/lua-5.1.5/etc/lua.hpp"

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

json get_installed_modules(const std::string &dcs_install_path, const std::string &module_subdir) {
    json installed_modules;
    if (std::filesystem::exists(dcs_install_path + module_subdir)) {
        for (const auto &dir : std::filesystem::directory_iterator(dcs_install_path + module_subdir)) {
            const std::string module_abs_path = dir.path().string();
            const auto mods_subdir_str_loc = module_abs_path.find(module_subdir);
            installed_modules.push_back(
                module_abs_path.substr(mods_subdir_str_loc + module_subdir.size(), module_abs_path.size()));
        }
    }
    return installed_modules;
}

json get_clickabledata(const std::string &dcs_install_path,
                       const std::string &module_name,
                       const std::string &lua_script) {
    json clickabledata_items;

    // create new Lua state
    lua_State *lua_state;
    lua_state = luaL_newstate();

    // load Lua libraries
    luaL_openlibs(lua_state);

    // Write variables to lua, the below sends to lua: [module_name = "A-10C"]
    lua_pushstring(lua_state, dcs_install_path.c_str());
    lua_setglobal(lua_state, "dcs_install_path");
    lua_pushstring(lua_state, module_name.c_str());
    lua_setglobal(lua_state, "module_name");

    // Run the lua script file, expecting multiple return values.
    const int lua_stack_size = lua_gettop(lua_state);
    const int file_status = luaL_loadfile(lua_state, lua_script.c_str());
    if (file_status != 0) {
        lua_close(lua_state);
        return clickabledata_items;
    }
    const int script_status = lua_pcall(lua_state, 0, LUA_MULTRET, 0);
    if (script_status != 0) {
        lua_close(lua_state);
        return clickabledata_items;
    }

    // Handle each of the return values one at a time.
    while ((lua_gettop(lua_state) - lua_stack_size) > 0) {
        clickabledata_items.push_back(lua_tostring(lua_state, lua_gettop(lua_state)));
        lua_pop(lua_state, 1);
    }

    // close the Lua state
    lua_close(lua_state);

    return clickabledata_items;
}
