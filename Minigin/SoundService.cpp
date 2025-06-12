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
#include <memory>

#include "ResourceManager.h"

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
            std::cerr << "[\033[31mERROR\033[0m] SDL_mixer initialization failed: " << Mix_GetError() << std::endl;
        }
        // Allow 16 sound channels for effects
        Mix_AllocateChannels(16);

        m_basePath = dae::ResourceManager::GetInstance().GetDataPath();

        m_Worker = std::thread(&Impl::ProcessQueue, this);
    }

    ~Impl()
    {
        // First set the stop flag to true
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_Stop = true;
        }

        // Notify the worker thread to check the stop flag
        m_CondVar.notify_all();

        // Wait for the worker thread to finish
        if (m_Worker.joinable())
        {
            m_Worker.join();
        }

        // Stop any playing music
        if (Mix_PlayingMusic())
        {
            Mix_HaltMusic();
        }

        // Clear current music path
        m_CurrentMusicPath.clear();

        // Free all sound chunks
        m_SoundCache.clear();

        // Free music, including current music
        m_CurrentMusic.reset();

        // Clean up music cache
        m_MusicCache.clear();

        // Close SDL_mixer
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

    std::filesystem::path m_basePath;

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
            // Combine base path with the provided path
            std::filesystem::path fullPath = m_basePath / filePath;
            auto* chunk = Mix_LoadWAV(fullPath.string().c_str());
            if (chunk)
            {
                m_SoundCache[filePath] = ChunkPtr(chunk, Mix_FreeChunk);
                std::cout << "[\033[33mDebug\033[0m] Sound loaded successfully: " << fullPath.string() << "\n";
            }
            else
            {
                std::cerr << "[\033[33mDebug\033[0m] Failed to load sound: " << fullPath.string()
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
            Mix_VolumeChunk(m_SoundCache[filePath].get(), volume);
            Mix_PlayChannel(-1, m_SoundCache[filePath].get(), 0);
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
            // Combine base path with the provided path
            std::filesystem::path fullPath = m_basePath / filePath;
            auto* music = Mix_LoadMUS(fullPath.string().c_str());
            if (music)
            {
                m_MusicCache[filePath] = MusicPtr(music, Mix_FreeMusic);
                std::cout << "[\033[33mDebug\033[0m] Music loaded successfully: " << fullPath.string() << std::endl;
            }
            else
            {
                std::cerr << "[\033[31mERROR\033[0m] Failed to load music: " << fullPath.string()
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
            Mix_PlayMusic(m_MusicCache[filePath].get(), loop ? -1 : 1);
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
            std::cout << "[\033[33mDebug\033[0m] Stopped music" << "\n";
        }
    }

    void PauseMusicInternal()
    {
        if (Mix_PlayingMusic() && !Mix_PausedMusic())
        {
            Mix_PauseMusic();
            std::cout << "[\033[33mDebug\033[0m] Paused music" << "\n";
        }
    }

    void ResumeMusicInternal()
    {
        if (Mix_PausedMusic())
        {
            Mix_ResumeMusic();
            std::cout << "[\033[33mDebug\033[0m] Resumed music" << "\n";
        }
    }

    // Use shared_ptr with custom deleter for audio resources
    using ChunkPtr = std::shared_ptr<Mix_Chunk>;
    using MusicPtr = std::shared_ptr<Mix_Music>;

    std::unordered_map<std::string, ChunkPtr> m_SoundCache;
    std::unordered_map<std::string, MusicPtr> m_MusicCache;
    std::queue<SoundRequest> m_Queue;
    std::mutex m_Mutex;
    std::condition_variable m_CondVar;
    std::thread m_Worker;
    std::atomic<bool> m_Stop;

    // Music tracking
    MusicPtr m_CurrentMusic;
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
    std::cout << "[\033[33mDebug\033[0m] CALLED OutputSound with path: " << filePath << "\n";
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

void SoundService::Shutdown()
{
    // Force clean shutdown by resetting the implementation
    m_Impl.reset();
}