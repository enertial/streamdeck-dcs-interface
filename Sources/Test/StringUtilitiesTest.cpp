// Copyright 2020 Charles Tytler

#include "gtest/gtest.h"

#include "../Utilities/StringUtilities.cpp"

namespace test {

TEST(StringUtilitiesTest, is_integer_for_int) {
    EXPECT_TRUE(is_integer("25"));
    EXPECT_TRUE(is_integer("-25"));
    EXPECT_TRUE(is_integer("0"));
}

TEST(StringUtilitiesTest, is_integer_with_leading_spaces) {
    EXPECT_TRUE(is_integer(" 25"));
    EXPECT_EQ(std::stoi(" 25"), 25);
    EXPECT_TRUE(is_integer("  -25"));
    EXPECT_EQ(std::stoi("  -25"), -25);
    EXPECT_TRUE(is_integer("   0"));
    EXPECT_EQ(std::stoi("   0"), 0);
}

TEST(StringUtilitiesTest, is_integer_with_trailing_spaces) {
    EXPECT_TRUE(is_integer("25    "));
    EXPECT_EQ(std::stoi("25    "), 25);
    EXPECT_TRUE(is_integer("-25  "));
    EXPECT_EQ(std::stoi("-25  "), -25);
    EXPECT_TRUE(is_integer("0   "));
    EXPECT_EQ(std::stoi("0   "), 0);
}

TEST(StringUtilitiesTest, is_integer_with_only_spaces) {
    EXPECT_FALSE(is_integer(" "));
    EXPECT_FALSE(is_integer("    "));
}

TEST(StringUtilitiesTest, is_integer_with_alpha_chars) {
    EXPECT_FALSE(is_integer("25a"));
    EXPECT_FALSE(is_integer("b-25"));
    EXPECT_FALSE(is_integer("c0"));
}

TEST(StringUtilitiesTest, is_integer_with_decimal) {
    EXPECT_FALSE(is_integer("25.4"));
    EXPECT_FALSE(is_integer(".25"));
    EXPECT_FALSE(is_integer("0."));
    EXPECT_FALSE(is_integer("16.0"));
}

TEST(StringUtilitiesTest, is_number_for_int) {
    EXPECT_TRUE(is_number("25"));
    EXPECT_TRUE(is_number("-25"));
    EXPECT_TRUE(is_number("0"));
}

TEST(StringUtilitiesTest, is_number_with_leading_spaces) {
    EXPECT_TRUE(is_number(" 25"));
    EXPECT_EQ(std::stod(" 25"), 25);
    EXPECT_TRUE(is_number("  -25"));
    EXPECT_EQ(std::stod("  -25"), -25);
    EXPECT_TRUE(is_number("   0"));
    EXPECT_EQ(std::stod("   0"), 0);
}

TEST(StringUtilitiesTest, is_number_with_trailing_spaces) {
    EXPECT_TRUE(is_number("25    "));
    EXPECT_EQ(std::stod("25   "), 25);
    EXPECT_TRUE(is_number("-25  "));
    EXPECT_EQ(std::stod("-25  "), -25);
    EXPECT_TRUE(is_number("0   "));
    EXPECT_EQ(std::stod("0   "), 0);
}

TEST(StringUtilitiesTest, is_number_with_only_spaces) {
    EXPECT_FALSE(is_number(" "));
    EXPECT_FALSE(is_number("    "));
}

TEST(StringUtilitiesTest, is_number_with_alpha_chars) {
    EXPECT_FALSE(is_number("25a"));
    EXPECT_FALSE(is_number("b-25"));
    EXPECT_FALSE(is_number("c0"));
}

TEST(StringUtilitiesTest, is_number_with_decimal) {
    EXPECT_TRUE(is_number("25.4"));
    EXPECT_TRUE(is_number(".25"));
    EXPECT_TRUE(is_number("0."));
    EXPECT_TRUE(is_number("16.0"));
}

TEST(StringUtilitiesTest, pop_key_and_value_on_empty) {
    std::stringstream ss;
    std::pair<std::string, std::string> key_and_value;
    ss << "";
    EXPECT_FALSE(pop_key_and_value(ss, ',', '=', key_and_value));
}

TEST(StringUtilitiesTest, pop_key_and_value_single_token) {
    std::stringstream ss;
    std::pair<std::string, std::string> key_and_value;
    ss << "key=value";
    EXPECT_TRUE(pop_key_and_value(ss, ',', '=', key_and_value));
    EXPECT_EQ("key", key_and_value.first);
    EXPECT_EQ("value", key_and_value.second);
    EXPECT_FALSE(pop_key_and_value(ss, ',', '=', key_and_value));
}

TEST(StringUtilitiesTest, pop_key_and_value_multiple_tokens) {
    std::stringstream ss;
    std::pair<std::string, std::string> key_and_value;
    ss << "key1=value1,key2=value2";
    EXPECT_TRUE(pop_key_and_value(ss, ',', '=', key_and_value));
    EXPECT_EQ("key1", key_and_value.first);
    EXPECT_EQ("value1", key_and_value.second);
    EXPECT_TRUE(pop_key_and_value(ss, ',', '=', key_and_value));
    EXPECT_EQ("key2", key_and_value.first);
    EXPECT_EQ("value2", key_and_value.second);
    EXPECT_FALSE(pop_key_and_value(ss, ',', '=', key_and_value));
}

TEST(StringUtilitiesTest, pop_key_and_value_missing_key) {
    std::stringstream ss;
    std::pair<std::string, std::string> key_and_value;
    ss << "key1=value1,=value2";
    EXPECT_TRUE(pop_key_and_value(ss, ',', '=', key_and_value));
    EXPECT_EQ("key1", key_and_value.first);
    EXPECT_EQ("value1", key_and_value.second);
    EXPECT_FALSE(pop_key_and_value(ss, ',', '=', key_and_value));
}

TEST(StringUtilitiesTest, pop_key_and_value_empty_value) {
    std::stringstream ss;
    std::pair<std::string, std::string> key_and_value;
    ss << "key1=,key2=value2";
    EXPECT_TRUE(pop_key_and_value(ss, ',', '=', key_and_value));
    EXPECT_EQ("key1", key_and_value.first);
    EXPECT_EQ("", key_and_value.second);
    EXPECT_TRUE(pop_key_and_value(ss, ',', '=', key_and_value));
    EXPECT_EQ("key2", key_and_value.first);
    EXPECT_EQ("value2", key_and_value.second);
    EXPECT_FALSE(pop_key_and_value(ss, ',', '=', key_and_value));
}

TEST(StringUtilitiesTest, pop_key_and_value_missing_key_value_delim) {
    std::stringstream ss;
    std::pair<std::string, std::string> key_and_value;
    ss << "key1value1";
    EXPECT_FALSE(pop_key_and_value(ss, ',', '=', key_and_value));
}

TEST(StringUtilitiesTest, pop_key_and_value_missing_token_delim) {
    std::stringstream ss;
    std::pair<std::string, std::string> key_and_value;
    ss << "key1=value1key2=value2";
    EXPECT_TRUE(pop_key_and_value(ss, ',', '=', key_and_value));
    EXPECT_EQ("key1", key_and_value.first);
    EXPECT_EQ("value1key2=value2", key_and_value.second);
}

} // namespace test