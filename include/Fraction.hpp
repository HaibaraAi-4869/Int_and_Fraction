#ifndef FRACTION_HPP
#define FRACTION_HPP
#include <Int.hpp>
class Fraction
{
private:
    Int numerator, denominator;
    void reduce()
    {
        Int t = gcd(abs(numerator), denominator);
        numerator /= t;
        denominator /= t;
    }

public:
    Fraction(const Int &num, const Int &den) : numerator(0), denominator(1)
    {
        if (den.is_zero())
            try
            {
                throw std::runtime_error("denominator can't be zero!");
            }
            catch (const std::runtime_error &e)
            {
                std::cerr << e.what() << '\n';
                throw;
            }
        numerator = num;
        denominator = den;
        if (!den.sgn)
        {
            numerator = -numerator;
            denominator.sgn = true;
        }
        reduce();
    }
    Fraction(long long num = 0, long long den = 1) : Fraction(Int(num), Int(den)) {}
    explicit operator bool() const noexcept { return bool(numerator); }
    friend constexpr bool operator==(const Fraction &lhs, const Fraction &rhs) { return lhs.numerator == rhs.numerator && lhs.denominator == rhs.denominator; }
    friend constexpr bool operator!=(const Fraction &lhs, const Fraction &rhs) { return !(lhs == rhs); }
    friend bool operator<(const Fraction &lhs, const Fraction &rhs) { return lhs.numerator * rhs.denominator < lhs.denominator * rhs.numerator; }
    friend bool operator>(const Fraction &lhs, const Fraction &rhs) { return rhs < lhs; }
    friend bool operator>=(const Fraction &lhs, const Fraction &rhs) { return !(lhs < rhs); }
    friend bool operator<=(const Fraction &lhs, const Fraction &rhs) { return !(rhs < lhs); }
    Fraction operator-() const
    {
        Fraction res = *this;
        res.numerator = -res.numerator;
        return res;
    }
    Fraction reciprocal() const { return Fraction(denominator, numerator); }
    Fraction &operator+=(const Fraction &rhs)
    {
        numerator = numerator * rhs.denominator + denominator * rhs.numerator;
        denominator *= rhs.denominator;
        reduce();
        return *this;
    }
    Fraction &operator-=(const Fraction &rhs) { return *this += -rhs; }
    Fraction &operator*=(const Fraction &rhs)
    {
        numerator *= rhs.numerator;
        denominator *= rhs.denominator;
        reduce();
        return *this;
    }
    Fraction &operator/=(const Fraction &rhs) { return *this *= rhs.reciprocal(); }
    friend Fraction operator+(const Fraction &lhs, const Fraction &rhs)
    {
        Fraction res = lhs;
        return res += rhs;
    }
    friend Fraction operator-(const Fraction &lhs, const Fraction &rhs)
    {
        Fraction res = lhs;
        return res -= rhs;
    }
    friend Fraction operator*(const Fraction &lhs, const Fraction &rhs)
    {
        Fraction res = lhs;
        return res *= rhs;
    }
    friend Fraction operator/(const Fraction &lhs, const Fraction &rhs)
    {
        Fraction res = lhs;
        return res /= rhs;
    }
    friend Fraction abs(const Fraction &x) { return x.numerator.sgn ? x : -x; }
    friend Fraction pow(Fraction a, Int b)
    {
        Fraction res = 1;
        for (; b; b.divided_by_two())
        {
            if (b[0] & 1)
                res *= a;
            a *= a;
        }
        return res;
    }
    friend std::istream &operator>>(std::istream &is, Fraction &rhs)
    {
        Int num, den;
        is >> num >> den;
        rhs = Fraction(num, den);
        return is;
    }
    friend std::ostream &operator<<(std::ostream &os, const Fraction &rhs)
    {
        os << rhs.numerator;
        if (rhs.denominator != 1)
            os << '/' << rhs.denominator;
        return os;
    }
};
#endif