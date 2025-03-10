#pragma once
#include <cmath> // Add this for std::sqrt

namespace dae
{
    class Command
    {
    public:
        virtual ~Command() = default;
        virtual void Execute() = 0;

        // Fixed overloads for analog input
        virtual void Execute(float /*value*/) { Execute(); }  // Parameter still unused but explicit
        virtual void Execute(float x, float y) { Execute(std::sqrt(x * x + y * y)); }
    };
}