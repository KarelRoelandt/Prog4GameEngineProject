// SoundService.cpp
#include "SoundService.h"
#include <SDL_mixer.h>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <atomic>
#include <iostream>

enum class SoundRequestType { LoadSound, PlaySound, LoadMusic, PlayMusic, StopMusic, PauseMusic, ResumeMusic };

struct SoundRequest
{
    SoundRequestType type{ SoundRequestType::LoadSound };
    std::string filePath;
    int volume{ MIX_MAX_VOLUME };
    bool loop{ false };
};

class SoundService::Impl
{
public:
    Impl() : m_Stop(false), m_CurrentMusic(nullptr)
    {
        // Initialize SDL_mixer
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0)
        {
            std::cerr << "[ERROR] SDL_mixer initialization failed: " << Mix_GetError() << std::endl;
        }
        // Allow 16 sound channels for effects
        Mix_AllocateChannels(16);

        m_Worker = std::thread(&Impl::ProcessQueue, this);
    }

    ~Impl()
    {
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_Stop = true;
        }
        m_CondVar.notify_one();
        if (m_Worker.joinable()) m_Worker.join();

        // Free all sound chunks
        for (auto& pair : m_SoundCache)
        {
            Mix_FreeChunk(pair.second);
        }

        // Free music
        if (m_CurrentMusic)
        {
            Mix_FreeMusic(m_CurrentMusic);
            m_CurrentMusic = nullptr;  // Set to nullptr after freeing
        }

        // Clean up music cache, avoiding double-free
        for (auto& pair : m_MusicCache)
        {
            // Only free if it's not the current music (which was already freed)
            if (pair.second != nullptr && pair.second != m_CurrentMusic)
            {
                Mix_FreeMusic(pair.second);
            }
        }

        Mix_CloseAudio();
    }

    void Enqueue(SoundRequestType type, const std::string& file, int volume = MIX_MAX_VOLUME, bool loop = false)
    {
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_Queue.push({ type, file, volume, loop });
        }
        m_CondVar.notify_one();
    }

private:
    void ProcessQueue()
    {
        while (true)
        {
            SoundRequest req;
            {
                std::unique_lock<std::mutex> ul(m_Mutex);
                m_CondVar.wait(ul, [this] { return m_Stop || !m_Queue.empty(); });
                if (m_Stop && m_Queue.empty()) break;
                req = m_Queue.front();
                m_Queue.pop();
            }

            switch (req.type)
            {
            case SoundRequestType::LoadSound:
                LoadSoundInternal(req.filePath);
                break;

            case SoundRequestType::PlaySound:
                PlaySoundInternal(req.filePath, req.volume);
                break;

            case SoundRequestType::LoadMusic:
                LoadMusicInternal(req.filePath);
                break;

            case SoundRequestType::PlayMusic:
                PlayMusicInternal(req.filePath, req.volume, req.loop);
                break;

            case SoundRequestType::StopMusic:
                StopMusicInternal();
                break;

            case SoundRequestType::PauseMusic:
                PauseMusicInternal();
                break;

            case SoundRequestType::ResumeMusic:
                ResumeMusicInternal();
                break;
            }
        }
    }

    void LoadSoundInternal(const std::string& filePath)
    {
        if (m_SoundCache.find(filePath) == m_SoundCache.end())
        {
            auto* chunk = Mix_LoadWAV(filePath.c_str());
            if (chunk)
            {
                m_SoundCache[filePath] = chunk;
                std::cout << "[\033[33mDebug\033[0m]] Sound loaded successfully: " << filePath << std::endl;
            }
            else
            {
                std::cerr << "[\033[33mDebug\033[0m] Failed to load sound: " << filePath
                    << " - " << Mix_GetError() << std::endl;
            }
        }
        else
        {
            std::cout << "[\033[33mDebug\033[0m] Sound already loaded: " << filePath << std::endl;
        }
    }

    void PlaySoundInternal(const std::string& filePath, int volume)
    {
        if (m_SoundCache.find(filePath) != m_SoundCache.end())
        {
            Mix_VolumeChunk(m_SoundCache[filePath], volume);
            Mix_PlayChannel(-1, m_SoundCache[filePath], 0);
            std::cout << "[\033[33mDebug\033[0m] Playing sound: " << filePath << " at volume: " << volume << std::endl;
        }
        else
        {
            std::cerr << "[\033[31mERROR\033[0m] Sound not loaded, cannot play: " << filePath << "\n";
        }
    }

    void LoadMusicInternal(const std::string& filePath)
    {
        if (m_MusicCache.find(filePath) == m_MusicCache.end())
        {
            auto* music = Mix_LoadMUS(filePath.c_str());
            if (music)
            {
                m_MusicCache[filePath] = music;
                std::cout << "[\033[33mDebug\033[0m] Music loaded successfully: " << filePath << std::endl;
            }
            else
            {
                std::cerr << "[\033[31mERROR\033[0m] Failed to load music: " << filePath
                    << " - " << Mix_GetError() << std::endl;
            }
        }
        else
        {
            std::cout << "[\033[33mDebug\033[0m] Music already loaded: " << filePath << std::endl;
        }
    }

    void PlayMusicInternal(const std::string& filePath, int volume, bool loop)
    {
        // First stop any currently playing music
        StopMusicInternal();

        // Load the music if it's not already loaded
        if (m_MusicCache.find(filePath) == m_MusicCache.end())
        {
            LoadMusicInternal(filePath);
        }

        // Play the music if it's loaded
        if (m_MusicCache.find(filePath) != m_MusicCache.end())
        {
            Mix_VolumeMusic(volume);
            Mix_PlayMusic(m_MusicCache[filePath], loop ? -1 : 1);
            m_CurrentMusicPath = filePath;
            m_CurrentMusic = m_MusicCache[filePath];
            std::cout << "[\033[33mDebug\033[0m] Playing music: " << filePath << " at volume: " << volume << (loop ? " (looping)" : "") << std::endl;
        }
    }

    void StopMusicInternal()
    {
        if (Mix_PlayingMusic())
        {
            Mix_HaltMusic();
            std::cout << "[\033[33mDebug\033[0m] Stopped music" << std::endl;
        }
    }

    void PauseMusicInternal()
    {
        if (Mix_PlayingMusic() && !Mix_PausedMusic())
        {
            Mix_PauseMusic();
            std::cout << "[\033[33mDebug\033[0m] Paused music" << std::endl;
        }
    }

    void ResumeMusicInternal()
    {
        if (Mix_PausedMusic())
        {
            Mix_ResumeMusic();
            std::cout << "[\033[33mDebug\033[0m] Resumed music" << std::endl;
        }
    }

    std::unordered_map<std::string, Mix_Chunk*> m_SoundCache;
    std::unordered_map<std::string, Mix_Music*> m_MusicCache;
    std::queue<SoundRequest> m_Queue;
    std::mutex m_Mutex;
    std::condition_variable m_CondVar;
    std::thread m_Worker;
    std::atomic<bool> m_Stop;

    // Music tracking
    Mix_Music* m_CurrentMusic;
    std::string m_CurrentMusicPath;
};

SoundService::SoundService() : m_Impl(std::make_unique<Impl>()) {}
SoundService::~SoundService() = default;

void SoundService::LoadSound(const std::string& filePath)
{
    m_Impl->Enqueue(SoundRequestType::LoadSound, filePath);
}

void SoundService::OutputSound(const std::string& filePath, int volume)
{
    m_Impl->Enqueue(SoundRequestType::PlaySound, filePath, volume);
}

void SoundService::LoadMusic(const std::string& filePath)
{
    m_Impl->Enqueue(SoundRequestType::LoadMusic, filePath);
}

void SoundService::PlayMusic(const std::string& filePath, int volume, bool loop)
{
    m_Impl->Enqueue(SoundRequestType::PlayMusic, filePath, volume, loop);
}

void SoundService::StopMusic()
{
    m_Impl->Enqueue(SoundRequestType::StopMusic, "");
}

void SoundService::PauseMusic()
{
    m_Impl->Enqueue(SoundRequestType::PauseMusic, "");
}

void SoundService::ResumeMusic()
{
    m_Impl->Enqueue(SoundRequestType::ResumeMusic, "");
}

bool SoundService::IsMusicPlaying()
{
    return Mix_PlayingMusic() && !Mix_PausedMusic();
}