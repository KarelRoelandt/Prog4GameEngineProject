// ISoundService.h
#pragma once
#include <string>

class ISoundService
{
public:
    virtual ~ISoundService() = default;

    // Sound effects methods
    virtual void LoadSound(const std::string& filePath) = 0;
    virtual void OutputSound(const std::string& filePath, int volume) = 0;

    // Music methods
    virtual void LoadMusic(const std::string& filePath) = 0;
    virtual void PlayMusic(const std::string& filePath, int volume, bool loop = true) = 0;
    virtual void StopMusic() = 0;
    virtual void PauseMusic() = 0;
    virtual void ResumeMusic() = 0;
    virtual bool IsMusicPlaying() = 0;

    // Add the Shutdown method to the interface
    virtual void Shutdown() = 0;
};