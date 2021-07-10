// Copyright 2020 Charles Tytler

#pragma once

#include "../Vendor/json/src/json.hpp"
using json = nlohmann::json;

/**
 * @brief Get the installed modules within provided DCS installation directory.
 *
 * @param dcs_install_path Path to DCS World installation (e.g. "C:\Program Files\Eagle Dynamics\DCS World")
 * @param module_subdir    Subdirectory within installation directory that contains module folders ("/mods/aircraft/")
 * @return json            Json array of identified files
 */
json get_installed_modules(const std::string& dcs_install_path, const std::string& module_subdir);

/**
 * @brief Extract clickabledata elements from a DCS World module.
 *
 * @param dcs_install_path Path to DCS World installation (e.g. "C:\Program Files\Eagle Dynamics\DCS World")
 * @param module_name      Name of the module matching the folder naming convention (e.g. "A-10C")
 * @param lua_script       Lua script to run which should return comma-separated strings of clickabledata attributes.
 * @return json            Json array of comma-separated strings, each array element is one clickabledata element.
 */
json get_clickabledata(const std::string& dcs_install_path,
                       const std::string& module_name,
                       const std::string& lua_script);