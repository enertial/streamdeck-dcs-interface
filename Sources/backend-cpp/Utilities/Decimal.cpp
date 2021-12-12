// Copyright 2020 Charles Tytler

#include "Decimal.h"

#include <algorithm>

Decimal::Decimal() : significant_digits_(0), exponent_(0) {}
Decimal::Decimal(std::string number) { string_to_decimal(number); }
Decimal::Decimal(int significant_digits, int exponent) : significant_digits_(significant_digits), exponent_(exponent) {}

std::string Decimal::str() const
{
    std::string value_as_decimal = std::to_string(significant_digits_);
    if (exponent_ > 0) {
        size_t neg_sign_offset = (significant_digits_ < 0) ? 1 : 0;
        while (value_as_decimal.size() - neg_sign_offset <= exponent_) {
            // Add leading zeros if necessary.
            value_as_decimal.insert(neg_sign_offset, "0");
        }
        value_as_decimal.insert(value_as_decimal.size() - exponent_, ".");
    }
    return value_as_decimal;
}

void Decimal::string_to_decimal(std::string number)
{
    const auto decimal_loc = number.find(".");
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

int Decimal::get_as_higher_exponent(const int higher_exponent) const
{
    int significant_digits_at_higher_exponent = significant_digits_;
    for (int i = exponent_; i < higher_exponent; ++i) {
        significant_digits_at_higher_exponent *= 10;
    }
    return significant_digits_at_higher_exponent;
}

/**
 *  Operator Overloads
 */

Decimal operator+(const Decimal &lhs, const Decimal &rhs)
{
    const int precision = (std::max)(lhs.exponent_, rhs.exponent_);
    return Decimal(lhs.get_as_higher_exponent(precision) + rhs.get_as_higher_exponent(precision), precision);
}

Decimal operator-(const Decimal &lhs, const Decimal &rhs)
{
    const int precision = (std::max)(lhs.exponent_, rhs.exponent_);
    return Decimal(lhs.get_as_higher_exponent(precision) - rhs.get_as_higher_exponent(precision), precision);
}

bool operator<(const Decimal &lhs, const Decimal &rhs)
{
    const int common_precision = (std::max)(lhs.exponent_, rhs.exponent_);
    return lhs.get_as_higher_exponent(common_precision) < rhs.get_as_higher_exponent(common_precision);
}

bool operator==(const Decimal &lhs, const Decimal &rhs)
{
    const int common_precision = (std::max)(lhs.exponent_, rhs.exponent_);
    return lhs.get_as_higher_exponent(common_precision) == rhs.get_as_higher_exponent(common_precision);
}