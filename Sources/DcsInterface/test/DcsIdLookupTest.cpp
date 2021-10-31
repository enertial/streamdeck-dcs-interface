// Copyright 2020 Charles Tytler

#include "gtest/gtest.h"

#include "DcsInterface/DcsIdLookup.h"
#include "ElgatoSD/EPLJSONUtils.h"

namespace test
{
TEST(DcsIdLookupTest, get_installed_modules_bad_path)
{
    const std::string dcs_install_path = "non-existant-path";
    const std::string module_subdir = "/Test/";
    json found_files_and_result = get_installed_modules(dcs_install_path, module_subdir);
    EXPECT_EQ("DCS Install path [non-existant-path/Test/] not found.", found_files_and_result["result"]);
    EXPECT_EQ(0, found_files_and_result["installed_modules"].size());
}

TEST(DcsIdLookupTest, nonexistant_lua_file)
{
    // Test that a bad path will return an empty json container.
    const std::string lua_script = "non-existant-file.lua";
    json returned_values = get_clickabledata("path", "module", lua_script);
    EXPECT_EQ("Lua file load error (6): cannot open non-existant-file.lua: No such file or directory",
              EPLJSONUtils::GetStringByName(returned_values, "result"));
    EXPECT_EQ(0, returned_values["clickabledata_items"].size());
}

} // namespace test