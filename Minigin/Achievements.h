#pragma once
#include <steam_api.h>

struct Achievement_t
{
    int m_eAchievementID;
    const char* m_pchAchievementID;
    char m_rgchName[128];
    char m_rgchDescription[256];
    bool m_bAchieved;
    int m_iIconImage;
};

class CSteamAchievements
{
private:
    int64 m_iAppID;
    Achievement_t* m_pAchievements;
    int m_iNumAchievements;
    bool m_bInitialized;

    // Add CCallback members
    CCallback<CSteamAchievements, UserStatsReceived_t, false> m_CallbackUserStatsReceived;
    CCallback<CSteamAchievements, UserStatsStored_t, false> m_CallbackUserStatsStored;
    CCallback<CSteamAchievements, UserAchievementStored_t, false> m_CallbackAchievementStored;

public:
    CSteamAchievements(Achievement_t* Achievements, int NumAchievements);
    ~CSteamAchievements();
    bool RequestStats();
    bool SetAchievement(const char* ID);

    // Remove STEAM_CALLBACK macros
    void OnUserStatsReceived(UserStatsReceived_t* pCallback);
    void OnUserStatsStored(UserStatsStored_t* pCallback);
    void OnAchievementStored(UserAchievementStored_t* pCallback);
};
