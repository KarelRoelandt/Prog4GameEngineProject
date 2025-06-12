// NullSoundService.h
#pragma once
#include "ISoundService.h"

class NullSoundService final : public ISoundService
{
public:
    void LoadSound(const std::string&) override {}
    void OutputSound(const std::string&, int) override {}

    void LoadMusic(const std::string&) override {}
    void PlayMusic(const std::string&, int, bool) override {}
    void StopMusic() override {}
    void PauseMusic() override {}
    void ResumeMusic() override {}
    bool IsMusicPlaying() override { return false; }

    // Add the Shutdown method to the interface
    virtual void Shutdown() override {};
};