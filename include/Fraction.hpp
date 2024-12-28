#ifndef FRACTION_HPP
#define FRACTION_HPP

#include <Int.hpp>

template <class T = int>
class Fraction
{
private:
    Int<T> numerator, denominator;

    void f()
    {
        Int t = gcd(abs(numerator), denominator);
        numerator /= t, denominator /= t;
    }

public:
    Fraction(const Int<T> &num = 0, const Int<T> &den = 1) : numerator(num), denominator(den)
    {
        if (den.is_zero())
            try
            {
                throw std::runtime_error("denominator can't be zero!");
            }
            catch(const std::runtime_error& e)
            {
                std::cerr << e.what() << '\n';
                throw;
            }
        if (den.sgn == false)
        {
            denominator.sgn ^= 1;
            numerator = -numerator;
        }
        f();
    }
    explicit operator bool() const noexcept { return numerator; }
    Fraction operator-() const { return Fraction(-numerator, denominator); }
    Fraction reciprocal() const
    {
        if (numerator.is_zero())
            try
            {
                throw std::runtime_error("zero has no reciprocal!");
            }
            catch(const std::runtime_error& e)
            {
                std::cerr << e.what() << '\n';
                throw;
            }
        return Fraction(denominator, numerator);    
    }
    Fraction &operator+=(const Fraction &rhs)
    {
        numerator *= rhs.denominator;
        numerator += denominator * rhs.numerator;
        denominator *= rhs.denominator;
        f();
        return *this;
    }
    Fraction &operator-=(const Fraction &rhs) { return *this += -rhs; }
    Fraction &operator*=(const Fraction &rhs)
    {
        numerator *= rhs.numerator;
        denominator *= rhs.denominator;
        f();
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
    friend std::istream &operator>>(std::istream &is, Fraction &rhs)
    {
        Int num, den;
        is >> num >> den;
        rhs = Fraction(num, den);
        return is;
    }
    friend std::ostream &operator<<(std::ostream &os, const Fraction &rhs)
    {
        os << rhs.numerator << '/' << rhs.denominator;
        return os;
    }
};

#endif