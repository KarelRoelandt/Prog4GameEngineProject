// Event.h
#pragma once

namespace dae
{
    enum class EventType
    {
        PlayerDied,
        HealthChanged,
        ScoreChanged 
    };

    class Event
    {
    public:
        explicit Event(EventType type) : m_Type(type) {}
        EventType GetType() const { return m_Type; }

    private:
        EventType m_Type;
    };
}
