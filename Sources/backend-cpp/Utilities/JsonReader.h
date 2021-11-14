// Copyright 2021 Charles Tytler

#pragma once

#include <optional>
#include <string>

#include "Vendor/json/src/json.hpp"
using json = nlohmann::json;

std::optional<json> get_module_list(const std::string &path);

std::optional<json> read_json_file(const std::string &filename);
