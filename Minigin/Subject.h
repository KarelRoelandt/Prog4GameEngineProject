#pragma once
#include <vector>
#include <algorithm>
#include "Observer.h"

namespace dae
{
    class Subject
    {
    public:
        virtual ~Subject() = default;

        void AddObserver(Observer* observer) {
            observers.push_back(observer);
        }

        void RemoveObserver(Observer* observer) {
            observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
        }

        void Notify(const Event& event) {
            for (Observer* observer : observers) {
                observer->OnNotify(event);
            }
        }

    private:
        std::vector<Observer*> observers;
    };
}