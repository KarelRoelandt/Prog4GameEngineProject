#include "Achievements.h"
#include <cstring>

CSteamAchievements::CSteamAchievements(Achievement_t* Achievements, int NumAchievements)
    : m_pAchievements(Achievements), m_iNumAchievements(NumAchievements), m_bInitialized(false),
    m_CallbackUserStatsReceived(this, &CSteamAchievements::OnUserStatsReceived),
    m_CallbackUserStatsStored(this, &CSteamAchievements::OnUserStatsStored),
    m_CallbackAchievementStored(this, &CSteamAchievements::OnAchievementStored)
{
    m_iAppID = SteamUtils()->GetAppID();
    RequestStats();
}

CSteamAchievements::~CSteamAchievements()
{
}

bool CSteamAchievements::RequestStats()
{
    if (!SteamUserStats() || !SteamUser())
        return false;

    if (!SteamUserStats()->RequestCurrentStats())
        return false;

    return true;
}

bool CSteamAchievements::SetAchievement(const char* ID)
{
    if (SteamUserStats()->SetAchievement(ID))
    {
        return SteamUserStats()->StoreStats();
    }
    return false;
}

void CSteamAchievements::OnUserStatsReceived(UserStatsReceived_t* pCallback)
{
    if (static_cast<uint64>(m_iAppID) == pCallback->m_nGameID)
    {
        if (pCallback->m_eResult == k_EResultOK)
        {
            m_bInitialized = true;
        }
    }
}

void CSteamAchievements::OnUserStatsStored(UserStatsStored_t* pCallback)
{
    if (static_cast<uint64>(m_iAppID) == pCallback->m_nGameID)
    {
        if (pCallback->m_eResult == k_EResultOK)
        {
            // Stats stored successfully
        }
    }
}

void CSteamAchievements::OnAchievementStored(UserAchievementStored_t* pCallback)
{
    if (static_cast<uint64>(m_iAppID) == pCallback->m_nGameID)
    {
        // Achievement stored successfully
    }
}

