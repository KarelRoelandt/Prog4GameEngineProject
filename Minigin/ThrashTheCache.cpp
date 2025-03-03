#include "ThrashTheCache.h"
#include <imgui_plot.h>

struct Transform
{
    float matrix[16] = { 1, 0, 0, 0,
                         0, 1, 0, 0,
                         0, 0, 1, 0,
                         0, 0, 0, 1 };
};

class GameObject
{
public:
    Transform transform;
    int id;
};

class GameObjectAlt
{
public:
    Transform* transform;
    int id;
};


// Add a flag to indicate whether the calculation is in progress
bool isCalculating = false;

void ThrashTheCache::CalculateEx1()
{
    isCalculating = true; // Set the flag to true when calculation starts

    const size_t buffer_size = 1 << 26; // 2^26
    std::vector<int> arr(buffer_size, 1); // Initialize with 1 for simplicity

    m_Results1.clear();

    for (size_t stepsize = 1; stepsize <= 1024; stepsize *= 2)
    {
        std::vector<int> results(20);
        for (int i = 0; i < 20; ++i)
        {
            auto start = std::chrono::high_resolution_clock::now();
            for (size_t j = 0; j < buffer_size; j += stepsize)
            {
                arr[j] *= 2;
            }
            auto end = std::chrono::high_resolution_clock::now();
            results[i] = static_cast<int>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());

            // Check for negative results
            if (results[i] < 0)
            {
                results[i] = 0; // Handle appropriately
            }
        }

        int sum = std::accumulate(results.begin(), results.end(), 0);
        int average = sum / static_cast<int>(results.size());
        std::cout << "Step size: " << stepsize << ", Average time (ns): " << average << std::endl;
        m_Results1.emplace_back(static_cast<int>(stepsize), average);
    }

    isCalculating = false; // Set the flag to false when calculation ends
}

void ThrashTheCache::CalculateEx2()
{

    std::vector<GameObject> arr(10000000);

    m_Results2.clear();

    for (size_t stepsize = 1; stepsize <= 1024; stepsize *= 2)
    {
        std::vector<int> results(20);
        for (int i = 0; i < 20; i++)
        {
            auto start = std::chrono::steady_clock::now();
            for (size_t j = 0; j < arr.size(); j += stepsize)
            {
                arr[j].id *= 2;
            }
            auto end = std::chrono::steady_clock::now();
            results[i] = static_cast<int>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
        }

        int sum = std::accumulate(results.begin(), results.end(), 0);
        int average = sum / static_cast<int>(results.size());
        m_Results2.emplace_back(static_cast<int>(stepsize), average);
    }
}

void ThrashTheCache::CalculateEx2Alt()
{
    std::vector<GameObjectAlt> arr(10000000);

    m_Results3.clear();

    for (size_t stepsize = 1; stepsize <= 1024; stepsize *= 2)
    {
        std::vector<int> results(20);
        for (int i = 0; i < 20; i++)
        {
            auto start = std::chrono::steady_clock::now();
            for (size_t j = 0; j < arr.size(); j += stepsize)
            {
                arr[j].id *= 2;
            }
            auto end = std::chrono::steady_clock::now();
            results[i] = static_cast<int>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
        }

        int sum = std::accumulate(results.begin(), results.end(), 0);
        int average = sum / static_cast<int>(results.size());
        m_Results3.emplace_back(static_cast<int>(stepsize), average);
    }
}


void ThrashTheCache::RenderImGui() const
{
    if (ImGui::Begin("Exercise 1"))
    {
        std::vector<float> xValues, yValues;
        for (const auto& result : m_Results1)
        {
            xValues.push_back(static_cast<float>(result.first));
            yValues.push_back(static_cast<float>(result.second));
        }

        // Text input field
        static char buffer[64] = "10";

        ImGui::InputText("# samples", buffer, sizeof(buffer));

        // Button to invoke CalculateEx1
        if (isCalculating)
        {
            ImGui::Button("Wait for it...");
        }
        else
        {
            if (ImGui::Button("Trash the cache"))
            {
                const_cast<ThrashTheCache*>(this)->CalculateEx1();
            }
        }

        if (!xValues.empty() && !yValues.empty())
        {
            ImGui::PlotLines("##", yValues.data(), static_cast<int>(yValues.size()), 0, nullptr, FLT_MAX, FLT_MAX, ImVec2(0, 200));
        }

        ImGui::End();
    }

    if (ImGui::Begin("Exercise 2"))
    {
        std::vector<float> xValues1, yValues1;
        for (const auto& result : m_Results2)
        {
            xValues1.push_back(static_cast<float>(result.first));
            yValues1.push_back(static_cast<float>(result.second));
        }
        std::vector<float> xValues2, yValues2;
        for (const auto& result : m_Results3)
        {
            xValues2.push_back(static_cast<float>(result.first));
            yValues2.push_back(static_cast<float>(result.second));
        }

        // Text input field
        static char buffer[64] = "100";

        ImGui::InputText("# samples", buffer, sizeof(buffer));

        // Button to invoke CalculateEx2
        if (ImGui::Button("Trash the cache with GameObject3D"))
        {
            const_cast<ThrashTheCache*>(this)->CalculateEx2();
        }
        if (!xValues1.empty() && !yValues1.empty())
        {
            ImGui::PlotLines("##", yValues1.data(), static_cast<int>(yValues1.size()), 0, nullptr, FLT_MAX, FLT_MAX, ImVec2(0, 200));
        }

        
        if (ImGui::Button("Trash the cache with GameObject3DAlt"))
        {
            const_cast<ThrashTheCache*>(this)->CalculateEx2Alt();
        }

        if (!xValues2.empty() && !yValues2.empty())
        {
            ImGui::PlotLines("##", yValues2.data(), static_cast<int>(yValues2.size()), 0, nullptr, FLT_MAX, FLT_MAX, ImVec2(0, 200));
        }

        if (!xValues1.empty() && !yValues1.empty() && !xValues2.empty() && !yValues2.empty())
        {
	        // Print combined text above the graph
	        ImGui::Text("Combined:");

	        ImGui::PlotConfig conf;
	        conf.values.xs = xValues1.data();
	        conf.values.ys_list = new const float* [2] { yValues1.data(), yValues2.data() };
	        conf.values.count = static_cast<int>(yValues1.size());
	        conf.values.ys_count = 2;
	        conf.scale.min = 0;
	        conf.scale.max = std::max(*std::max_element(yValues1.begin(), yValues1.end()), *std::max_element(yValues2.begin(), yValues2.end()));
	        conf.tooltip.show = true;
	        conf.grid_x.show = true;
	        conf.grid_y.show = true;
	        conf.frame_size = ImVec2(400, 200);
	        conf.line_thickness = 2.f;
	        ImGui::Plot("##", conf);

	        // Clean up dynamically allocated memory
	        delete[] conf.values.ys_list;
		}

        
        ImGui::End();
    }
}