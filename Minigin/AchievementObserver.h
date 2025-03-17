// AchievementObserver.h
#pragma once
#include "Observer.h"
#include "Achievements.h"

namespace dae
{
    class AchievementObserver : public Observer
    {
    public:
        AchievementObserver(CSteamAchievements* steamAchievements)
            : m_SteamAchievements(steamAchievements) {
        }

        void OnNotify(const GameObject* /*entity*/, int value) override
        {
            if (value == 0) // Health reaches 0
            {
                m_SteamAchievements->SetAchievement("ACH_WIN_ONE_GAME");
            }
            // Add more conditions as needed
        }

    private:
        CSteamAchievements* m_SteamAchievements;
    };
}