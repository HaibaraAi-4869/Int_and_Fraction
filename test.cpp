#include <chrono>
#include <cstdio>
#include <Fraction.hpp>
#include <Fraction_temp.hpp>
using namespace std;
void f1()
{

}
void f2()
{

}
int main()
{
    int T = 100, res = 0;
    while (T--)
    {
        auto l = chrono::high_resolution_clock::now();
        f1();
        auto mid = chrono::high_resolution_clock::now();
        f2();
        auto r = chrono::high_resolution_clock::now();
        res += mid - l < r - mid ? 1 : -1;
    }
    if (res >= 15)
        puts("Accept the first one!");
    else if (res <= -15)
        puts("Accept the second one!");
    else
        puts("Uncertain!");
}