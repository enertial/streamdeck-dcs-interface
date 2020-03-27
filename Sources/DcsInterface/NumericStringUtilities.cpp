// Copyright 2020 Charles Tytler

#include "pch.h"

#include "NumericStringUtilities.h"
#include <algorithm>
#include <math.h>

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

Decimal operator+(const Decimal &lhs, const Decimal &rhs) {
    int precision = (std::max)(lhs.exponent_, rhs.exponent_);
    int lhs_significant_digits = lhs.significant_digits_;
    int rhs_significant_digits = rhs.significant_digits_;
    for (int i = lhs.exponent_; i < precision; ++i) {
        lhs_significant_digits *= 10;
    }
    for (int i = rhs.exponent_; i < precision; ++i) {
        rhs_significant_digits *= 10;
    }
    return Decimal(lhs_significant_digits + rhs_significant_digits, precision);
}

Decimal operator-(const Decimal &lhs, const Decimal &rhs) {
    int precision = (std::max)(lhs.exponent_, rhs.exponent_);
    int lhs_significant_digits = lhs.significant_digits_;
    int rhs_significant_digits = rhs.significant_digits_;
    for (int i = lhs.exponent_; i < precision; ++i) {
        lhs_significant_digits *= 10;
    }
    for (int i = rhs.exponent_; i < precision; ++i) {
        rhs_significant_digits *= 10;
    }
    return Decimal(lhs_significant_digits - rhs_significant_digits, precision);
}
