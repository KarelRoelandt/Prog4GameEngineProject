#pragma once
#include <string>

class ISoundService
{
public:
    virtual ~ISoundService() = default;
    virtual void LoadSound(const std::string& filePath) = 0;
    virtual void OutputSound(const std::string& filePath, int volume) = 0;
};