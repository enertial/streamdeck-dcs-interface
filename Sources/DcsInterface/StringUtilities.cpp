// Copyright 2020 Charles Tytler

#include "pch.h"

#include "StringUtilities.h"
#include <algorithm>
#include <math.h>

bool is_integer(const std::string &str) {
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

bool is_number(const std::string &str) {
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

bool pop_key_and_value(std::stringstream &ss,
                       const char token_delim,
                       const char key_value_delim,
                       std::pair<std::string, std::string> &key_and_value) {
    // Iterate through tokens received from single message.
    std::string token;
    if (std::getline(ss, token, token_delim)) {
        // Parse token string of the form:
        //   "key<key_value_delim>value"
        const auto key_value_delim_loc = token.find(key_value_delim);
        if (key_value_delim_loc != std::string::npos && key_value_delim_loc > 0) {
            key_and_value.first = token.substr(0, key_value_delim_loc);
            key_and_value.second = token.substr(key_value_delim_loc + 1, token.size());
            return true;
        }
    }
    return false;
}

Decimal::Decimal() : significant_digits_(0), exponent_(0) {}

Decimal::Decimal(std::string number) { string_to_decimal(number); }
Decimal::Decimal(int significant_digits, int exponent) : significant_digits_(significant_digits), exponent_(exponent) {}

std::string Decimal::str() {
    std::string value_as_decimal = std::to_string(significant_digits_);
    if (exponent_ > 0) {
        size_t neg_sign_offset = (significant_digits_ < 0) ? 1 : 0;
        while (value_as_decimal.size() - neg_sign_offset <= exponent_) {
            // Add leading zeros if necessary.
            value_as_decimal.insert(neg_sign_offset, "0");
        }
        value_as_decimal.insert(value_as_decimal.size() - exponent_, ".");
    }
    return std::move(value_as_decimal);
}

void Decimal::string_to_decimal(std::string number) {
    auto decimal_loc = number.find(".");
    if (decimal_loc == std::string::npos) {
        // No decimal point in string, assume integer.
        significant_digits_ = std::stoi(number);
        exponent_ = 0;
    } else {
        // Find last digit, ignoring trailing 0's or spaces.
        auto last_digit_loc = number.find_last_not_of("0 ");
        exponent_ = static_cast<int>(last_digit_loc - decimal_loc);
        std::string digits_only = number.substr(0, last_digit_loc + 1).erase(decimal_loc, 1);
        significant_digits_ = std::stoi(digits_only);
    }
}

int Decimal::get_as_higher_exponent(const int higher_exponent) const {
    int significant_digits_at_higher_exponent = significant_digits_;
    for (int i = exponent_; i < higher_exponent; ++i) {
        significant_digits_at_higher_exponent *= 10;
    }
    return significant_digits_at_higher_exponent;
}

/**
 *  Operator Overloads
 */

Decimal operator+(const Decimal &lhs, const Decimal &rhs) {
    int precision = (std::max)(lhs.exponent_, rhs.exponent_);
    return Decimal(lhs.get_as_higher_exponent(precision) + rhs.get_as_higher_exponent(precision), precision);
}

Decimal operator-(const Decimal &lhs, const Decimal &rhs) {
    int precision = (std::max)(lhs.exponent_, rhs.exponent_);
    return Decimal(lhs.get_as_higher_exponent(precision) - rhs.get_as_higher_exponent(precision), precision);
}

bool operator<(const Decimal &lhs, const Decimal &rhs) {
    int common_precision = (std::max)(lhs.exponent_, rhs.exponent_);
    return lhs.get_as_higher_exponent(common_precision) < rhs.get_as_higher_exponent(common_precision);
}

bool operator==(const Decimal &lhs, const Decimal &rhs) {
    int common_precision = (std::max)(lhs.exponent_, rhs.exponent_);
    return lhs.get_as_higher_exponent(common_precision) == rhs.get_as_higher_exponent(common_precision);
}