// Copyright 2020 Charles Tytler

#pragma once

#include <string>

/**
 * @brief Provides a type for decimal values which can be converted to/from string representation and supports
 * basic arithmetic operations (except division) and comparison while maintaining precision.
 *
 * @throws Exception if string cannot be converted to a decimal representation.
 *
 */
class Decimal
{
  public:
    Decimal();
    Decimal(std::string number);
    Decimal(int integer);

    /**
     * @brief Returns a string representation of the decimal value.
     */
    std::string str() const;

    /**
     * @brief Returns a double representation of the decimal value.
     */
    double as_double() const;

    /**
     * @brief Overloaded summation operators allow add and subtract while maintaining precision of highest-exponent
     * Decimal.
     */
    friend Decimal operator+(const Decimal &lhs, const Decimal &rhs);
    friend Decimal operator-(const Decimal &lhs, const Decimal &rhs);
    friend Decimal operator*(const Decimal &lhs, const Decimal &rhs);
    inline Decimal &operator+=(const Decimal &rhs)
    {
        *this = *this + rhs;
        return *this;
    }
    inline Decimal &operator-=(const Decimal &rhs)
    {
        *this = *this - rhs;
        return *this;
    }
    inline Decimal &operator*=(const Decimal &rhs)
    {
        *this = *this * rhs;
        return *this;
    }

    friend bool operator<(const Decimal &lhs, const Decimal &rhs);
    friend inline bool operator>(const Decimal &lhs, const Decimal &rhs) { return rhs < lhs; }
    friend inline bool operator<=(const Decimal &lhs, const Decimal &rhs) { return !(lhs > rhs); }
    friend inline bool operator>=(const Decimal &lhs, const Decimal &rhs) { return !(lhs < rhs); }

    friend bool operator==(const Decimal &lhs, const Decimal &rhs);
    friend inline bool operator!=(const Decimal &lhs, const Decimal &rhs) { return !(lhs == rhs); }

  private:
    // Private constructor can directly set the exponent value.
    Decimal(int significant_digits, unsigned int exponent);

    int significant_digits_; // The significant digits of the decimal value.
    unsigned int exponent_;  // Exponent such that the numeric value = significant_digits * 10 ^ -(exponent).

    /**
     * @brief Converts a string to a Decimal represtentation (significant_digits and exponent).
     *
     * @param number String representing a numeric value.
     *
     * @throws invalid_argument exception (received from std::stoi) if unable to convert to numeric value.
     */
    void string_to_decimal(std::string number);

    /**
     * @brief Get significant digit representation with a higher exponent value.
     *        Note: it is assumed that current exponent_ <= higher_exponent.
     *
     * @param higher_exponent An exponent value (higher or equal to current exponent_).
     * @return Equivalent significant digits representation when using a higher exponent value.
     */
    int get_as_higher_exponent(const int higher_exponent) const;
};