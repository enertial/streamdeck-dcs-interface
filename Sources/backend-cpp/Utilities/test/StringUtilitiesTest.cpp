// Copyright 2020 Charles Tytler

#include "gtest/gtest.h"

#include "Utilities/StringUtilities.h"

namespace test
{
TEST(StringUtilitiesTest, is_integer_for_int)
{
    EXPECT_TRUE(is_integer("25"));
    EXPECT_TRUE(is_integer("-25"));
    EXPECT_TRUE(is_integer("0"));
}

TEST(StringUtilitiesTest, is_integer_with_leading_spaces)
{
    EXPECT_TRUE(is_integer(" 25"));
    EXPECT_EQ(std::stoi(" 25"), 25);
    EXPECT_TRUE(is_integer("  -25"));
    EXPECT_EQ(std::stoi("  -25"), -25);
    EXPECT_TRUE(is_integer("   0"));
    EXPECT_EQ(std::stoi("   0"), 0);
}

TEST(StringUtilitiesTest, is_integer_with_trailing_spaces)
{
    EXPECT_TRUE(is_integer("25    "));
    EXPECT_EQ(std::stoi("25    "), 25);
    EXPECT_TRUE(is_integer("-25  "));
    EXPECT_EQ(std::stoi("-25  "), -25);
    EXPECT_TRUE(is_integer("0   "));
    EXPECT_EQ(std::stoi("0   "), 0);
}

TEST(StringUtilitiesTest, is_integer_with_only_spaces)
{
    EXPECT_FALSE(is_integer(" "));
    EXPECT_FALSE(is_integer("    "));
}

TEST(StringUtilitiesTest, is_integer_with_alpha_chars)
{
    EXPECT_FALSE(is_integer("25a"));
    EXPECT_FALSE(is_integer("b-25"));
    EXPECT_FALSE(is_integer("c0"));
}

TEST(StringUtilitiesTest, is_integer_with_decimal)
{
    EXPECT_FALSE(is_integer("25.4"));
    EXPECT_FALSE(is_integer(".25"));
    EXPECT_FALSE(is_integer("0."));
    EXPECT_FALSE(is_integer("16.0"));
}

TEST(StringUtilitiesTest, is_number_for_int)
{
    EXPECT_TRUE(is_number("25"));
    EXPECT_TRUE(is_number("-25"));
    EXPECT_TRUE(is_number("0"));
}

TEST(StringUtilitiesTest, is_number_with_leading_spaces)
{
    EXPECT_TRUE(is_number(" 25"));
    EXPECT_EQ(std::stod(" 25"), 25);
    EXPECT_TRUE(is_number("  -25"));
    EXPECT_EQ(std::stod("  -25"), -25);
    EXPECT_TRUE(is_number("   0"));
    EXPECT_EQ(std::stod("   0"), 0);
}

TEST(StringUtilitiesTest, is_number_with_trailing_spaces)
{
    EXPECT_TRUE(is_number("25    "));
    EXPECT_EQ(std::stod("25   "), 25);
    EXPECT_TRUE(is_number("-25  "));
    EXPECT_EQ(std::stod("-25  "), -25);
    EXPECT_TRUE(is_number("0   "));
    EXPECT_EQ(std::stod("0   "), 0);
}

TEST(StringUtilitiesTest, is_number_with_only_spaces)
{
    EXPECT_FALSE(is_number(" "));
    EXPECT_FALSE(is_number("    "));
}

TEST(StringUtilitiesTest, is_number_with_alpha_chars)
{
    EXPECT_FALSE(is_number("25a"));
    EXPECT_FALSE(is_number("b-25"));
    EXPECT_FALSE(is_number("c0"));
}

TEST(StringUtilitiesTest, is_number_with_decimal)
{
    EXPECT_TRUE(is_number("25.4"));
    EXPECT_TRUE(is_number(".25"));
    EXPECT_TRUE(is_number("0."));
    EXPECT_TRUE(is_number("16.0"));
}

TEST(StringUtilitiesTest, split_pair_valid)
{
    const auto string_pair = split_pair("key=value", '=');
    EXPECT_TRUE(string_pair);
    EXPECT_EQ("key", string_pair.value().first);
    EXPECT_EQ("value", string_pair.value().second);
}

TEST(StringUtilitiesTest, split_pair_on_empty)
{
    const auto string_pair = split_pair("", '=');
    EXPECT_FALSE(string_pair);
}

TEST(StringUtilitiesTest, split_pair_empty_first)
{
    const auto string_pair = split_pair("=value", '=');
    EXPECT_FALSE(string_pair);
}

TEST(StringUtilitiesTest, split_pair_empty_second)
{
    const auto string_pair = split_pair("key=", '=');
    EXPECT_TRUE(string_pair);
    EXPECT_EQ("key", string_pair.value().first);
    EXPECT_EQ("", string_pair.value().second);
}

TEST(StringUtilitiesTest, split_pair_missing_delim)
{
    const auto string_pair = split_pair("keyvalue", '=');
    EXPECT_FALSE(string_pair);
}

TEST(StringUtilitiesTest, pop_key_and_value_on_empty)
{
    std::stringstream ss;
    ss << "";
    EXPECT_FALSE(pop_key_and_value(ss, ',', '='));
}

TEST(StringUtilitiesTest, pop_key_and_value_single_token)
{
    std::stringstream ss;
    ss << "key=value";
    auto key_and_value = pop_key_and_value(ss, ',', '=');
    EXPECT_TRUE(key_and_value);
    EXPECT_EQ("key", key_and_value.value().first);
    EXPECT_EQ("value", key_and_value.value().second);
    key_and_value = pop_key_and_value(ss, ',', '=');
    EXPECT_FALSE(key_and_value);
}

TEST(StringUtilitiesTest, pop_key_and_value_multiple_tokens)
{
    std::stringstream ss;
    ss << "key1=value1,key2=value2";
    auto key_and_value = pop_key_and_value(ss, ',', '=');
    EXPECT_TRUE(key_and_value);
    EXPECT_EQ("key1", key_and_value.value().first);
    EXPECT_EQ("value1", key_and_value.value().second);
    key_and_value = pop_key_and_value(ss, ',', '=');
    EXPECT_TRUE(key_and_value);
    EXPECT_EQ("key2", key_and_value.value().first);
    EXPECT_EQ("value2", key_and_value.value().second);
    key_and_value = pop_key_and_value(ss, ',', '=');
    EXPECT_FALSE(key_and_value);
}

TEST(StringUtilitiesTest, pop_key_and_value_missing_key_value_delim)
{
    std::stringstream ss;
    ss << "key1value1";
    auto key_and_value = pop_key_and_value(ss, ',', '=');
    EXPECT_FALSE(key_and_value);
}

TEST(StringUtilitiesTest, pop_key_and_value_missing_token_delim)
{
    std::stringstream ss;
    ss << "key1=value1key2=value2";
    auto key_and_value = pop_key_and_value(ss, ',', '=');
    EXPECT_TRUE(key_and_value);
    EXPECT_EQ("key1", key_and_value.value().first);
    EXPECT_EQ("value1key2=value2", key_and_value.value().second);
}

} // namespace test