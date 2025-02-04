#ifndef INT_HPP
#define INT_HPP
#include <cmath>
#include <string>
#include <vector>
#include <complex>
#include <utility>
#include <iostream>
#include <algorithm>
#include <stdexcept>
class Fraction;
class Int
{
    friend class Fraction;
    friend Fraction abs(const Fraction &);
    friend Fraction pow(Fraction, Int);

private:
    static constexpr int p = 2, ppow = 100;
    static constexpr double pi = 3.141592653589793;
    bool sgn;
    std::vector<int> a;
    int &back() noexcept { return a.back(); }
    int back() const noexcept { return a.back(); }
    int &operator[](std::size_t i) noexcept { return a[i]; }
    int operator[](std::size_t i) const noexcept { return a[i]; }
    void handle_carrying() noexcept
    {
        for (std::size_t i = 0; i + 1 < a.size(); ++i)
        {
            a[i + 1] += a[i] / ppow;
            a[i] %= ppow;
        }
        if (!back())
            a.pop_back();
    }
    void handle_leading_zeros() noexcept
    {
        std::size_t i = a.size() - 1;
        while (!a[i] && i)
            --i;
        a.resize(i + 1);
    }
    void handle_borrowing() noexcept
    {
        for (std::size_t i = 0; i + 1 < a.size(); ++i)
            if (a[i] < 0)
            {
                --a[i + 1];
                a[i] += ppow;
            }
    }
    void FFT(std::vector<std::complex<double>> &a, std::size_t n, int op) const
    {
        std::vector<int> r(n);
        for (std::size_t i = 1; i < n; ++i)
        {
            r[i] = r[i >> 1] >> 1 | (i & 1 ? n >> 1 : 0);
            if (i < r[i])
                std::swap(a[i], a[r[i]]);
        }
        for (std::size_t i = 1; i < n; i <<= 1)
        {
            std::complex<double> w(std::cos(pi / i), op * std::sin(pi / i));
            for (std::size_t j = 0; j < n; j += i << 1)
            {
                std::complex<double> wk = 1;
                for (std::size_t k = 0; k < i; ++k, wk *= w)
                {
                    std::complex<double> t = wk * a[i | j | k];
                    a[i | j | k] = a[j | k] - t;
                    a[j | k] += t;
                }
            }
        }
        if (op == -1)
            for (std::size_t i = 0; i < n; ++i)
                a[i] /= n;
    }
    void multiplied_by_two()
    {
        for (std::size_t i = 0; i < a.size(); ++i)
            a[i] <<= 1;
        a.emplace_back(0);
        handle_carrying();
    }
    void divided_by_two() noexcept
    {
        int res = 0;
        for (std::size_t i = a.size(); i; --i)
        {
            res = res * ppow + a[i - 1];
            a[i - 1] = res >> 1;
            res &= 1;
        }
        if (!a.back() && a.size() != 1)
            a.pop_back();
    }

public:
    Int(long long x = 0) : sgn(true), a()
    {
        if (x < 0)
        {
            x = -x;
            sgn = false;
        }
        do
            a.emplace_back(x % ppow);
        while (x /= ppow);
    }
    Int(const std::string &s) : Int()
    {
        bool sgn = s[0] != '-';
        std::vector<int> b((s.size() - !sgn + p - 1) / p);
        for (std::size_t i = 0, j = s.size() - p; i + 1 < b.size(); ++i, j -= p)
            b[i] = std::stoi(s.substr(j, p));
        std::size_t t = (s.size() - !sgn) % p;
        b.back() = std::stoi(s.substr(!sgn, t ? t : p));
        a = std::move(b);
        handle_leading_zeros();
        this->sgn = *this ? sgn : true;
    }
    explicit operator bool() const noexcept { return a[0] || a.size() != 1; }
    std::size_t size() const noexcept { return p * (a.size() - 1) + std::to_string(back()).size(); }
    friend bool operator==(const Int &lhs, const Int &rhs) { return lhs.sgn == rhs.sgn && lhs.a == rhs.a; }
    friend bool operator!=(const Int &lhs, const Int &rhs) { return !(lhs == rhs); }
    friend bool operator<(const Int &lhs, const Int &rhs)
    {
        if (lhs.sgn != rhs.sgn)
            return rhs.sgn;
        if (lhs.a.size() < rhs.a.size())
            return lhs.sgn;
        if (lhs.a.size() > rhs.a.size())
            return !lhs.sgn;
        for (std::size_t i = lhs.a.size(); i; --i)
            if (lhs[i - 1] < rhs[i - 1])
                return lhs.sgn;
            else if (lhs[i - 1] > rhs[i - 1])
                return !lhs.sgn;
        return false;
    }
    friend bool operator>(const Int &lhs, const Int &rhs) { return rhs < lhs; }
    friend bool operator>=(const Int &lhs, const Int &rhs) { return !(lhs < rhs); }
    friend bool operator<=(const Int &lhs, const Int &rhs) { return !(rhs < lhs); }
    Int operator-() const
    {
        Int ans = *this;
        if (ans)
            ans.sgn ^= 1;
        return ans;
    }
    Int &operator+=(const Int &rhs)
    {
        if (!rhs)
            return *this;
        if (sgn != rhs.sgn)
            return *this -= -rhs;
        std::size_t t;
        if (a.size() >= rhs.a.size())
            t = rhs.a.size();
        else
        {
            t = a.size();
            a.insert(a.end(), rhs.a.begin() + a.size(), rhs.a.end());
        }
        a.emplace_back(0);
        for (std::size_t i = 0; i < t; ++i)
            a[i] += rhs[i];
        handle_carrying();
        return *this;
    }
    Int &operator-=(const Int &rhs)
    {
        if (!rhs)
            return *this;
        if (sgn != rhs.sgn)
            return *this += -rhs;
        sgn = *this >= rhs;
        if (sgn ^ rhs.sgn)
        {
            std::size_t t = a.size();
            a.insert(a.end(), rhs.a.begin() + a.size(), rhs.a.end());
            for (std::size_t i = 0; i < t; ++i)
                a[i] = rhs[i] - a[i];
        }
        else
            for (std::size_t i = 0; i < rhs.a.size(); ++i)
                a[i] -= rhs[i];
        handle_borrowing();
        handle_leading_zeros();
        return *this;
    }
    Int &operator*=(const Int &rhs)
    {
        if (!*this || !rhs)
            return *this = 0;
        sgn = !(sgn ^ rhs.sgn);
        if (a.size() < 140 || rhs.a.size() < 140)
        {
            std::vector<int> c(a.size() + rhs.a.size());
            for (std::size_t i = 0; i < a.size(); ++i)
                for (std::size_t j = 0; j < rhs.a.size(); ++j)
                    c[i + j] += a[i] * rhs[j];
            a = std::move(c);
        }
        else
        {
            std::size_t n = 1, t = std::max(a.size(), rhs.a.size()) << 1;
            while (n < t)
                n <<= 1;
            std::vector<std::complex<double>> c(n);
            for (std::size_t i = 0; i < a.size(); ++i)
                c[i].real(a[i]);
            for (std::size_t i = 0; i < rhs.a.size(); ++i)
                c[i].imag(rhs[i]);
            FFT(c, n, 1);
            for (std::size_t i = 0; i < n; ++i)
                c[i] *= c[i];
            FFT(c, n, -1);
            a.resize(a.size() + rhs.a.size());
            for (std::size_t i = 0; i < a.size(); ++i)
                a[i] = static_cast<int>(c[i].imag() / 2 + 0.5);
        }
        handle_carrying();
        return *this;
    }
    Int &operator/=(const Int &rhs)
    {
        if (!rhs)
            try
            {
                throw std::runtime_error("divisor can't be zero!");
            }
            catch (const std::runtime_error &e)
            {
                std::cerr << e.what() << '\n';
                throw;
            }
        bool sgn = !(this->sgn ^ rhs.sgn);
        Int rhs_t = rhs, t = 1;
        this->sgn = rhs_t.sgn = true;
        if (*this < rhs_t)
            return *this = 0;
        while (rhs_t <= *this)
        {
            rhs_t.multiplied_by_two();
            t.multiplied_by_two();
        }
        Int res;
        do
        {
            if (*this >= rhs_t)
            {
                res += t;
                *this -= rhs_t;
            }
            rhs_t.divided_by_two();
            t.divided_by_two();
        } while (t);
        res.sgn = sgn;
        return *this = res;
    }
    Int &operator%=(const Int &rhs)
    {
        if (!rhs)
            try
            {
                throw std::runtime_error("divisor can't be zero!");
            }
            catch (const std::runtime_error &e)
            {
                std::cerr << e.what() << '\n';
                throw;
            }
        bool sgn = this->sgn;
        Int rhs_t = rhs, t = 1;
        this->sgn = rhs_t.sgn = true;
        while (rhs_t <= *this)
        {
            rhs_t.multiplied_by_two();
            t.multiplied_by_two();
        }
        Int res;
        do
        {
            if (*this >= rhs_t)
            {
                res += t;
                *this -= rhs_t;
            }
            rhs_t.divided_by_two();
            t.divided_by_two();
        } while (t);
        this->sgn = sgn;
        return *this;
    }
    Int &operator++() { return *this += 1; }
    Int operator++(int)
    {
        Int res = *this;
        ++*this;
        return res;
    }
    friend Int operator+(const Int &lhs, const Int &rhs)
    {
        Int res = lhs;
        return res += rhs;
    }
    Int &operator--() { return *this -= 1; }
    Int operator--(int)
    {
        Int res = *this;
        --*this;
        return res;
    }
    friend Int operator-(const Int &lhs, const Int &rhs)
    {
        Int res = lhs;
        return res -= rhs;
    }
    friend Int operator*(const Int &lhs, const Int &rhs)
    {
        Int res = lhs;
        return res *= rhs;
    }
    friend Int operator/(const Int &lhs, const Int &rhs)
    {
        Int res = lhs;
        return res /= rhs;
    }
    friend Int operator%(const Int &lhs, const Int &rhs)
    {
        Int res = lhs;
        return res %= rhs;
    }
    friend Int abs(const Int &x) { return x.sgn ? x : -x; }
    friend Int gcd(Int a, Int b)
    {
        while (b)
        {
            Int r = a % b;
            a = b;
            b = r;
        }
        return a;
    }
    friend Int pow(Int a, Int b)
    {
        Int res = 1;
        for (; b; b.divided_by_two())
        {
            if (b[0] & 1)
                res *= a;
            a *= a;
        }
        return res;
    }
    Int &operator<<=(std::size_t rhs) { return *this *= pow(Int(2), rhs); }
    Int &operator>>=(std::size_t rhs) { return *this /= pow(Int(2), rhs); }
    Int operator<<(std::size_t rhs)
    {
        Int res = *this;
        return res <<= rhs;
    }
    Int operator>>(std::size_t rhs)
    {
        Int res = *this;
        return res >>= rhs;
    }
    friend std::istream &operator>>(std::istream &is, Int &rhs)
    {
        std::string s;
        is >> s;
        rhs = s;
        return is;
    }
    friend std::ostream &operator<<(std::ostream &os, const Int &rhs)
    {
        if (!rhs.sgn)
            os << '-';
        os << rhs.back();
        char c = os.fill('0');
        for (std::size_t i = rhs.a.size() - 1; i; --i)
            os.width(p), os << rhs[i - 1];
        os.fill(c);
        return os;
    }
};
#endif