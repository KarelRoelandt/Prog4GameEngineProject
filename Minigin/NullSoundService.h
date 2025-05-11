// NullSoundService.h
#pragma once
#include "ISoundService.h"

class NullSoundService : public ISoundService
{
public:
    void LoadSound(const std::string& /*filePath*/) override {}
    void OutputSound(const std::string& /*filePath*/, int /*volume*/) override {}
};
