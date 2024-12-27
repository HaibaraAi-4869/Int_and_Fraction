#ifndef INT_HPP
#define INT_HPP

#include <cmath>
#include <string>
#include <vector>
#include <complex>
#include <iostream>
#include <algorithm>
#include <stdexcept>

template <class>
class Fraction;

template <class T = int>
class Int
{
    friend class Fraction<T>;

private:
    static constexpr int p = 2; // 因FFT精度问题在算2^10^7时最多压2位
    static constexpr T ppow = 100;
    static constexpr double pi = 3.141592653589793;

    bool sgn;
    std::vector<T> a;

    constexpr T &back() noexcept { return a.back(); }
    constexpr T back() const noexcept { return a.back(); }
    constexpr T &operator[](std::size_t i) noexcept { return a[i]; }
    constexpr T operator[](std::size_t i) const noexcept { return a[i]; }
    constexpr bool is_zero() const noexcept { return a.size() == 1 && a[0] == 0; }
    void f()
    {
        std::size_t i;
        for (i = 0; i + 1 < a.size(); ++i)
            if (a[i] < 0)
                --a[i + 1], a[i] += ppow;
            else
                a[i + 1] += a[i] / ppow, a[i] %= ppow;
        while (!a[i] && i)
            --i;
        a.resize(i + 1);
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
                    std::complex<double> t = wk * a[i + j + k];
                    a[i + j + k] = a[j + k] - t;
                    a[j + k] += t;
                }
            }
        }
        if (op == -1)
            for (std::size_t i = 0; i < n; ++i)
                a[i] /= n;
    }
    void MultipliedByTwo()
    {
        for (std::size_t i = 0; i < a.size(); ++i)
            a[i] <<= 1;
        a.emplace_back(0);
        f();
    }
    void DividedByTwo()
    {
        T res = 0;
        for (std::size_t i = a.size(); i; --i)
        {
            res = res * ppow + a[i - 1];
            a[i - 1] = res >> 1;
            res &= 1;
        }
        f();
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
    explicit operator bool() const noexcept { return !is_zero(); }
    std::size_t size() const { return p * (a.size() - 1) + std::to_string(back()).size(); }
    friend constexpr bool operator==(const Int &lhs, const Int &rhs) { return lhs.sgn == rhs.sgn && lhs.a == rhs.a; }
    friend constexpr bool operator!=(const Int &lhs, const Int &rhs) { return !(lhs == rhs); }
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
        if (rhs.is_zero())
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
        f();
        return *this;
    }
    Int &operator-=(const Int &rhs)
    {
        if (rhs.is_zero())
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
        f();
        return *this;
    }
    Int &operator*=(const Int &rhs)
    {
        if (is_zero() || rhs.is_zero())
            return *this = 0;
        sgn = !(sgn ^ rhs.sgn);
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
            a[i] = int(c[i].imag() / 2 + 0.5);
        f();
        return *this;
    }
    Int &operator/=(const Int &rhs)
    {
        if (rhs.is_zero())
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
        while (rhs_t <= *this)
        {
            rhs_t.MultipliedByTwo();
            t.MultipliedByTwo();
        }
        Int res;
        do
        {
            if (*this >= rhs_t)
            {
                res += t;
                *this -= rhs_t;
            }
            rhs_t.DividedByTwo();
            t.DividedByTwo();
        } while (t);
        res.sgn = sgn;
        return *this = res;
    }
    Int &operator%=(const Int &rhs)
    {
        if (rhs.is_zero())
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
            rhs_t.MultipliedByTwo();
            t.MultipliedByTwo();
        }
        Int res;
        do
        {
            if (*this >= rhs_t)
            {
                res += t;
                *this -= rhs_t;
            }
            rhs_t.DividedByTwo();
            t.DividedByTwo();
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
        for (; b; b.DividedByTwo())
        {
            if (b[0] & 1) // when base is even
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
        rhs.sgn = s[0] != '-';
        rhs.a.resize((s.size() - !rhs.sgn + p - 1) / p);
        for (std::size_t i = 0, j = s.size() - p; i + 1 < rhs.a.size(); ++i, j -= p)
            rhs[i] = std::stoull(s.substr(j, p));
        std::size_t t = (s.size() - !rhs.sgn) % p;
        rhs.back() = std::stoull(s.substr(!rhs.sgn, t ? t : p));
        rhs.f();
        if (rhs.is_zero())
            rhs.sgn = true;
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