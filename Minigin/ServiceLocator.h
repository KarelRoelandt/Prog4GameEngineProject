// ServiceLocator.h
#pragma once
#include <memory>

class ISoundService;

class ServiceLocator
{
public:
    static void RegisterSoundService(std::shared_ptr<ISoundService> service);
    static std::shared_ptr<ISoundService> GetSoundService();

private:
    static std::shared_ptr<ISoundService> m_SoundService;
};