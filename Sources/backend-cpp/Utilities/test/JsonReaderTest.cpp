// Copyright 2021 Charles Tytler

#include "gtest/gtest.h"

#include "Utilities/JsonReader.h"

namespace test
{
TEST(JsonReaderTest, getModuleListNonexistantPath)
{
    const std::string path = "non-existant-path";
    const auto maybe_module_list = get_module_list(path);
    EXPECT_FALSE(maybe_module_list);
}

TEST(JsonReaderTest, getModuleTestPath)
{
    const std::string path = "Sources\\backend-cpp\\Utilities\\test";
    const auto maybe_module_list = get_module_list(path);
    ASSERT_TRUE(maybe_module_list);
    const auto module_list = maybe_module_list.value();
    EXPECT_EQ(module_list.size(), 1);
    EXPECT_EQ(module_list[0], "Sources\\backend-cpp\\Utilities\\test\\sample.json");
}

TEST(JsonReaderTest, getModuleUnderRecursiveTestPath)
{
    const std::string path = "Sources\\backend-cpp\\Utilities\\";
    const auto maybe_module_list = get_module_list(path);
    ASSERT_TRUE(maybe_module_list);
    const auto module_list = maybe_module_list.value();
    EXPECT_EQ(module_list.size(), 1);
    EXPECT_EQ(module_list[0], "Sources\\backend-cpp\\Utilities\\test\\sample.json");
}

TEST(JsonReaderTest, getModuleTestPathWithForwardSlashes)
{
    const std::string path = "Sources/backend-cpp/Utilities/test";
    const auto maybe_module_list = get_module_list(path);
    ASSERT_TRUE(maybe_module_list);
    const auto module_list = maybe_module_list.value();
    EXPECT_EQ(module_list.size(), 1);
    EXPECT_EQ(module_list[0], "Sources/backend-cpp/Utilities/test\\sample.json");
}

TEST(JsonReaderTest, getModuleTestPathWithForwardSlashesTrailing)
{
    const std::string path = "Sources/backend-cpp/Utilities/test/";
    const auto maybe_module_list = get_module_list(path);
    ASSERT_TRUE(maybe_module_list);
    const auto module_list = maybe_module_list.value();
    EXPECT_EQ(module_list.size(), 1);
    EXPECT_EQ(module_list[0], "Sources/backend-cpp/Utilities/test/sample.json");
}

TEST(JsonReaderTest, nonexistantJsonFile)
{
    const std::string json_file = "non-existant-file.json";
    const auto maybe_json = read_json_file(json_file);
    EXPECT_FALSE(maybe_json);
}

TEST(JsonReaderTest, readSampleJsonFile)
{
    const std::string json_file = "Sources/backend-cpp/Utilities/test/sample.json";
    const auto maybe_json = read_json_file(json_file);
    ASSERT_TRUE(maybe_json);
    const auto json_obj = maybe_json.value();
    EXPECT_EQ(json_obj["field1"]["a"], 1);
    EXPECT_EQ(json_obj["field2"]["d"], 4);
}

} // namespace test