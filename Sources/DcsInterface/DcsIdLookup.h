// Copyright 2020 Charles Tytler

#pragma once

#include "../Vendor/json/src/json.hpp"
using json = nlohmann::json;

json get_installed_modules(const std::string &dcs_install_path, const std::string &module_subdir);

json get_clickabledata(const std::string &dcs_install_path,
                       const std::string &module_name,
                       const std::string &lua_script);