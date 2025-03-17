// Observer.h
#pragma once
#include <vector>

namespace dae
{
    class GameObject;  // Forward declaration

    class Observer
    {
    public:
        virtual ~Observer() = default;
        virtual void OnNotify(const GameObject* entity, int value) = 0; 
    };

    class Subject
    {
    public:
        virtual ~Subject() = default;

        void AddObserver(Observer* observer)
        {
            m_observers.push_back(observer);
        }

        void RemoveObserver(Observer* observer)
        {
            m_observers.erase(std::remove(m_observers.begin(), m_observers.end(), observer), m_observers.end());
        }

    protected:
        void NotifyObservers(const GameObject* entity, int value)
        {
            for (auto observer : m_observers)
            {
                observer->OnNotify(entity, value);
            }
        }

    private:
        std::vector<Observer*> m_observers{};
    };
}