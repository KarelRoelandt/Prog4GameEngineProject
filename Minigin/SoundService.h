// SoundService.h
#pragma once
#include "ISoundService.h"
#include <memory>

class SoundService final : public ISoundService
{
public:
    SoundService();
    ~SoundService() override;

    void LoadSound(const std::string& filePath) override;
    void OutputSound(const std::string& filePath, int volume) override;

private:
    class Impl;
    std::unique_ptr<Impl> m_Impl;
};