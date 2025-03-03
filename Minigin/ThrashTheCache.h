#pragma once
#include <vector>
#include <chrono>
#include <iostream>
#include <numeric>
#include "imgui.h"

class ThrashTheCache
{
public:
    void CalculateEx1();
    void CalculateEx2();
    void CalculateEx2Alt();
    void RenderImGui() const;

private:
    std::vector<std::pair<int, int>> m_Results1; // Pair of step size and average time
    std::vector<std::pair<int, int>> m_Results2; // Pair of step size and average time
    std::vector<std::pair<int, int>> m_Results3; // Pair of step size and average time
};
