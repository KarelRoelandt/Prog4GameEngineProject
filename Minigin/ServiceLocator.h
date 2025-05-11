// ServiceLocator.h
#pragma once
#include <memory>

class ISoundService;

class ServiceLocator final
{
public:
    static void RegisterSoundService(const std::shared_ptr<ISoundService>& service);
    static std::shared_ptr<ISoundService> GetSoundService();

private:
    static std::shared_ptr<ISoundService> m_SoundService;
};