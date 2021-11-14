// Copyright 2021 Charles Tytler

#include "JsonReader.h"

#include <filesystem>
#include <fstream>

std::optional<json> get_module_list(const std::string &path)
{
    auto module_list = json::array();
    if (std::filesystem::exists(path)) {
        for (const auto &filename : std::filesystem::recursive_directory_iterator(path)) {
            if (filename.path().extension() == ".json") {
                module_list.push_back(filename.path().string());
            }
        }
    }
    return module_list.size() > 0 ? std::make_optional(module_list) : std::nullopt;
}

std::optional<json> read_json_file(const std::string &filename)
{
    std::ifstream ifs(filename);
    if (ifs) {
        return json::parse(ifs);
    }
    return std::nullopt;
}
