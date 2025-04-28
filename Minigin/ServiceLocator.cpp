// ServiceLocator.cpp
#include "ServiceLocator.h"
#include "ISoundService.h"

std::shared_ptr<ISoundService> ServiceLocator::m_SoundService = nullptr;

void ServiceLocator::RegisterSoundService(std::shared_ptr<ISoundService> service)
{
    m_SoundService = service;
}
std::shared_ptr<ISoundService> ServiceLocator::GetSoundService()
{
    return m_SoundService;
}