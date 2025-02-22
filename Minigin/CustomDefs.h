#pragma once

namespace dae
{
    struct Vector2
    {
        float x{ 0 }, y{ 0 };

        // Define the + operator for Vector2
        Vector2 operator+(const Vector2& other) const
        {
            return { x + other.x, y + other.y };
        }
    };
}
