// Copyright 2020 Charles Tytler

#pragma once

#include <stdlib.h>
#include <string>

/**
 * @brief Helper function to identify if a string represents an integer.
 *
 */
inline bool is_integer(const std::string &str) {
    if (str.empty()) {
        return false;
    } else {
        // Add special handling for stripping of trailing spaces
        const std::string str_trailing_spaces_stripped = str.substr(0, str.find_last_not_of(" ") + 1);
        // Check if all characters are base 10 digits (0-9).
        char *ptr_to_first_non_numeric_char;
        strtol(str_trailing_spaces_stripped.c_str(), &ptr_to_first_non_numeric_char, 10);
        return (*ptr_to_first_non_numeric_char == '\0');
    }
}

/**
 * @brief Helper function to identify if a string represents a (decimal) number.
 *
 */
inline bool is_number(const std::string &str) {
    if (str.empty()) {
        return false;
    } else {
        // Add special handling for stripping of trailing spaces
        const std::string str_trailing_spaces_stripped = str.substr(0, str.find_last_not_of(" ") + 1);
        // Check if all characters can be converted to a floating point number.
        char *ptr_to_first_non_numeric_char;
        strtof(str_trailing_spaces_stripped.c_str(), &ptr_to_first_non_numeric_char);
        return (*ptr_to_first_non_numeric_char == '\0');
    }
}

/**
 * @brief Provides a type for decimal values which can be converted to/from string representation and supports
 * arithmetic while maintaining precision.
 *
 */
class Decimal {
  public:
    Decimal();
    Decimal(std::string number);
    Decimal(int significant_digits, int exponent);

    /**
     * @brief Returns a string representation of the decimal value.
     *
     */
    std::string str();

    friend Decimal operator+(const Decimal &lhs, const Decimal &rhs);
    friend Decimal operator-(const Decimal &lhs, const Decimal &rhs);

  private:
    int significant_digits_;
    int exponent_;

    void string_to_decimal(std::string number);
};