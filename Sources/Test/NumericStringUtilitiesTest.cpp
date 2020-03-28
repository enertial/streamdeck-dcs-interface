// Copyright 2020 Charles Tytler

#include "gtest/gtest.h"

#include "../DcsInterface/NumericStringUtilities.cpp"

namespace test {

TEST(NumericStringUtilitiesTest, is_integer_for_int) {
    EXPECT_TRUE(is_integer("25"));
    EXPECT_TRUE(is_integer("-25"));
    EXPECT_TRUE(is_integer("0"));
}

TEST(NumericStringUtilitiesTest, is_integer_with_leading_spaces) {
    EXPECT_TRUE(is_integer(" 25"));
    EXPECT_EQ(std::stoi(" 25"), 25);
    EXPECT_TRUE(is_integer("  -25"));
    EXPECT_EQ(std::stoi("  -25"), -25);
    EXPECT_TRUE(is_integer("   0"));
    EXPECT_EQ(std::stoi("   0"), 0);
}

TEST(NumericStringUtilitiesTest, is_integer_with_trailing_spaces) {
    EXPECT_TRUE(is_integer("25    "));
    EXPECT_EQ(std::stoi("25    "), 25);
    EXPECT_TRUE(is_integer("-25  "));
    EXPECT_EQ(std::stoi("-25  "), -25);
    EXPECT_TRUE(is_integer("0   "));
    EXPECT_EQ(std::stoi("0   "), 0);
}

TEST(NumericStringUtilitiesTest, is_integer_with_alpha_chars) {
    EXPECT_FALSE(is_integer("25a"));
    EXPECT_FALSE(is_integer("b-25"));
    EXPECT_FALSE(is_integer("c0"));
}

TEST(NumericStringUtilitiesTest, is_integer_with_decimal) {
    EXPECT_FALSE(is_integer("25.4"));
    EXPECT_FALSE(is_integer(".25"));
    EXPECT_FALSE(is_integer("0."));
    EXPECT_FALSE(is_integer("16.0"));
}

TEST(NumericStringUtilitiesTest, is_number_for_int) {
    EXPECT_TRUE(is_number("25"));
    EXPECT_TRUE(is_number("-25"));
    EXPECT_TRUE(is_number("0"));
}

TEST(NumericStringUtilitiesTest, is_number_with_leading_spaces) {
    EXPECT_TRUE(is_number(" 25"));
    EXPECT_EQ(std::stod(" 25"), 25);
    EXPECT_TRUE(is_number("  -25"));
    EXPECT_EQ(std::stod("  -25"), -25);
    EXPECT_TRUE(is_number("   0"));
    EXPECT_EQ(std::stod("   0"), 0);
}

TEST(NumericStringUtilitiesTest, is_number_with_trailing_spaces) {
    EXPECT_TRUE(is_number("25    "));
    EXPECT_EQ(std::stod("25   "), 25);
    EXPECT_TRUE(is_number("-25  "));
    EXPECT_EQ(std::stod("-25  "), -25);
    EXPECT_TRUE(is_number("0   "));
    EXPECT_EQ(std::stod("0   "), 0);
}

TEST(NumericStringUtilitiesTest, is_number_with_alpha_chars) {
    EXPECT_FALSE(is_number("25a"));
    EXPECT_FALSE(is_number("b-25"));
    EXPECT_FALSE(is_number("c0"));
}

TEST(NumericStringUtilitiesTest, is_number_with_decimal) {
    EXPECT_TRUE(is_number("25.4"));
    EXPECT_TRUE(is_number(".25"));
    EXPECT_TRUE(is_number("0."));
    EXPECT_TRUE(is_number("16.0"));
}

TEST(NumericStringUtilitiesTest, Decimal_default_0) {
    Decimal decimal;
    EXPECT_EQ("0", decimal.str());
}

TEST(NumericStringUtilitiesTest, Decimal_int_int_constructor) {
    Decimal decimal(34, 3);
    EXPECT_EQ("0.034", decimal.str());
}

TEST(NumericStringUtilitiesTest, Decimal_convert_decimal_number) {
    Decimal decimal("0.00403");
    EXPECT_EQ("0.00403", decimal.str());
}

TEST(NumericStringUtilitiesTest, Decimal_convert_string_with_leading_trailing_zeros) {
    Decimal decimal("00000.004030000");
    EXPECT_EQ("0.00403", decimal.str());
}

TEST(NumericStringUtilitiesTest, Decimal_convert_integer) {
    Decimal decimal("576");
    EXPECT_EQ("576", decimal.str());
}

TEST(NumericStringUtilitiesTest, Decimal_copy) {
    Decimal decimal("0.01");
    Decimal decimal_copy = decimal;
    EXPECT_EQ("0.01", decimal_copy.str());
}

TEST(NumericStringUtilitiesTest, Decimal_move) {
    Decimal decimal("0.01");
    Decimal decimal_copy = std::move(decimal);
    EXPECT_EQ("0.01", decimal_copy.str());
}

TEST(NumericStringUtilitiesTest, Decimal_addition_equal_precision) {
    Decimal result = Decimal("0.01") + Decimal("0.14");
    EXPECT_EQ("0.15", result.str());
}

TEST(NumericStringUtilitiesTest, Decimal_addition_lhs_greater_precision) {
    Decimal result = Decimal("0.00071") + Decimal("0.14");
    EXPECT_EQ("0.14071", result.str());
}

TEST(NumericStringUtilitiesTest, Decimal_addition_rhs_greater_precision) {
    Decimal result = Decimal("0.01") + Decimal("0.100045");
    EXPECT_EQ("0.110045", result.str());
}

TEST(NumericStringUtilitiesTest, Decimal_subtraction_equal_precision) {
    Decimal result = Decimal("0.01") - Decimal("0.14");
    EXPECT_EQ("-0.13", result.str());
}

TEST(NumericStringUtilitiesTest, Decimal_subtraction_lhs_greater_precision) {
    Decimal result = Decimal("0.15544") - Decimal("0.022");
    EXPECT_EQ("0.13344", result.str());
}

TEST(NumericStringUtilitiesTest, Decimal_subtraction_rhs_greater_precision) {
    Decimal result = Decimal("0.01") - Decimal("0.0005");
    EXPECT_EQ("0.0095", result.str());
}

TEST(NumericStringUtilitiesTest, Decimal_plus_equals) {
    Decimal decimal("4.5");
    decimal += Decimal("0.2");
    EXPECT_EQ("4.7", decimal.str());
}

TEST(NumericStringUtilitiesTest, Decimal_minus_equals) {
    Decimal decimal("4.5");
    decimal -= Decimal("0.5");
    EXPECT_EQ("4.0", decimal.str());
}

TEST(NumericStringUtilitiesTest, Decimal_less_than) {
    EXPECT_TRUE(Decimal("3.556") < Decimal("5"));
    EXPECT_TRUE(Decimal("0.05") < Decimal("0.1"));
    EXPECT_FALSE(Decimal("12.1") < Decimal("0.9995"));
}

TEST(NumericStringUtilitiesTest, Decimal_equal_to) {
    EXPECT_TRUE(Decimal("3.556") == Decimal("03.55600"));
    EXPECT_TRUE(Decimal("0.05") == Decimal(".05"));
    EXPECT_TRUE(Decimal("123.42") == Decimal(12342, 2));
    EXPECT_FALSE(Decimal("55.0") == Decimal("550"));
}

} // namespace test