// ServiceLocator.cpp
#include "ServiceLocator.h"
#include "ISoundService.h"
#include "NullSoundService.h"

std::shared_ptr<ISoundService> ServiceLocator::m_SoundService = std::make_shared<NullSoundService>();

void ServiceLocator::RegisterSoundService(const std::shared_ptr<ISoundService>& service)
{
    m_SoundService = service ? service : std::make_shared<NullSoundService>();
}

std::shared_ptr<ISoundService> ServiceLocator::GetSoundService()
{
    return m_SoundService;
}
