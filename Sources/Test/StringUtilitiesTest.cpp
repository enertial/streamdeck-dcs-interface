// Copyright 2020 Charles Tytler

#include "gtest/gtest.h"

#include "../DcsInterface/StringUtilities.cpp"

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
    EXPECT_FALSE(pop_key_and_value(ss, ',', ':', key_and_value));
}

TEST(StringUtilitiesTest, pop_key_and_value_single_token) {
    std::stringstream ss;
    std::pair<std::string, std::string> key_and_value;
    ss << "key:value";
    EXPECT_TRUE(pop_key_and_value(ss, ',', ':', key_and_value));
    EXPECT_EQ("key", key_and_value.first);
    EXPECT_EQ("value", key_and_value.second);
    EXPECT_FALSE(pop_key_and_value(ss, ',', ':', key_and_value));
}

TEST(StringUtilitiesTest, pop_key_and_value_multiple_tokens) {
    std::stringstream ss;
    std::pair<std::string, std::string> key_and_value;
    ss << "key1:value1,key2:value2";
    EXPECT_TRUE(pop_key_and_value(ss, ',', ':', key_and_value));
    EXPECT_EQ("key1", key_and_value.first);
    EXPECT_EQ("value1", key_and_value.second);
    EXPECT_TRUE(pop_key_and_value(ss, ',', ':', key_and_value));
    EXPECT_EQ("key2", key_and_value.first);
    EXPECT_EQ("value2", key_and_value.second);
    EXPECT_FALSE(pop_key_and_value(ss, ',', ':', key_and_value));
}

TEST(StringUtilitiesTest, pop_key_and_value_missing_key) {
    std::stringstream ss;
    std::pair<std::string, std::string> key_and_value;
    ss << "key1:value1,:value2";
    EXPECT_TRUE(pop_key_and_value(ss, ',', ':', key_and_value));
    EXPECT_EQ("key1", key_and_value.first);
    EXPECT_EQ("value1", key_and_value.second);
    EXPECT_FALSE(pop_key_and_value(ss, ',', ':', key_and_value));
}

TEST(StringUtilitiesTest, pop_key_and_value_empty_value) {
    std::stringstream ss;
    std::pair<std::string, std::string> key_and_value;
    ss << "key1:,key2:value2";
    EXPECT_TRUE(pop_key_and_value(ss, ',', ':', key_and_value));
    EXPECT_EQ("key1", key_and_value.first);
    EXPECT_EQ("", key_and_value.second);
    EXPECT_TRUE(pop_key_and_value(ss, ',', ':', key_and_value));
    EXPECT_EQ("key2", key_and_value.first);
    EXPECT_EQ("value2", key_and_value.second);
    EXPECT_FALSE(pop_key_and_value(ss, ',', ':', key_and_value));
}

TEST(StringUtilitiesTest, pop_key_and_value_missing_key_value_delim) {
    std::stringstream ss;
    std::pair<std::string, std::string> key_and_value;
    ss << "key1value1";
    EXPECT_FALSE(pop_key_and_value(ss, ',', ':', key_and_value));
}

TEST(StringUtilitiesTest, pop_key_and_value_missing_token_delim) {
    std::stringstream ss;
    std::pair<std::string, std::string> key_and_value;
    ss << "key1:value1key2:value2";
    EXPECT_TRUE(pop_key_and_value(ss, ',', ':', key_and_value));
    EXPECT_EQ("key1", key_and_value.first);
    EXPECT_EQ("value1key2:value2", key_and_value.second);
}

TEST(StringUtilitiesTest, Decimal_default_0) {
    Decimal decimal;
    EXPECT_EQ("0", decimal.str());
}

TEST(StringUtilitiesTest, Decimal_int_int_constructor) {
    Decimal decimal(34, 3);
    EXPECT_EQ("0.034", decimal.str());
}

TEST(StringUtilitiesTest, Decimal_convert_decimal_number) {
    Decimal decimal("0.00403");
    EXPECT_EQ("0.00403", decimal.str());
}

TEST(StringUtilitiesTest, Decimal_convert_string_with_leading_trailing_zeros) {
    Decimal decimal("00000.004030000");
    EXPECT_EQ("0.00403", decimal.str());
}

TEST(StringUtilitiesTest, Decimal_convert_integer) {
    Decimal decimal("576");
    EXPECT_EQ("576", decimal.str());
}

TEST(StringUtilitiesTest, Decimal_copy) {
    Decimal decimal("0.01");
    Decimal decimal_copy = decimal;
    EXPECT_EQ("0.01", decimal_copy.str());
}

TEST(StringUtilitiesTest, Decimal_move) {
    Decimal decimal("0.01");
    Decimal decimal_copy = std::move(decimal);
    EXPECT_EQ("0.01", decimal_copy.str());
}

TEST(StringUtilitiesTest, Decimal_addition_equal_precision) {
    Decimal result = Decimal("0.01") + Decimal("0.14");
    EXPECT_EQ("0.15", result.str());
}

TEST(StringUtilitiesTest, Decimal_addition_lhs_greater_precision) {
    Decimal result = Decimal("0.00071") + Decimal("0.14");
    EXPECT_EQ("0.14071", result.str());
}

TEST(StringUtilitiesTest, Decimal_addition_rhs_greater_precision) {
    Decimal result = Decimal("0.01") + Decimal("0.100045");
    EXPECT_EQ("0.110045", result.str());
}

TEST(StringUtilitiesTest, Decimal_subtraction_equal_precision) {
    Decimal result = Decimal("0.01") - Decimal("0.14");
    EXPECT_EQ("-0.13", result.str());
}

TEST(StringUtilitiesTest, Decimal_subtraction_lhs_greater_precision) {
    Decimal result = Decimal("0.15544") - Decimal("0.022");
    EXPECT_EQ("0.13344", result.str());
}

TEST(StringUtilitiesTest, Decimal_subtraction_rhs_greater_precision) {
    Decimal result = Decimal("0.01") - Decimal("0.0005");
    EXPECT_EQ("0.0095", result.str());
}

TEST(StringUtilitiesTest, Decimal_plus_equals) {
    Decimal decimal("4.5");
    decimal += Decimal("0.2");
    EXPECT_EQ("4.7", decimal.str());
}

TEST(StringUtilitiesTest, Decimal_minus_equals) {
    Decimal decimal("4.5");
    decimal -= Decimal("0.5");
    EXPECT_EQ("4.0", decimal.str());
}

TEST(StringUtilitiesTest, Decimal_less_than) {
    EXPECT_TRUE(Decimal("3.556") < Decimal("5"));
    EXPECT_TRUE(Decimal("0.05") < Decimal("0.1"));
    EXPECT_FALSE(Decimal("12.1") < Decimal("0.9995"));
}

TEST(StringUtilitiesTest, Decimal_equal_to) {
    EXPECT_TRUE(Decimal("3.556") == Decimal("03.55600"));
    EXPECT_TRUE(Decimal("0.05") == Decimal(".05"));
    EXPECT_TRUE(Decimal("123.42") == Decimal(12342, 2));
    EXPECT_FALSE(Decimal("55.0") == Decimal("550"));
}

} // namespace test