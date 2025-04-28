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

enum class SoundRequestType { Load, Play };
struct SoundRequest
{
    SoundRequestType type{ SoundRequestType::Load }; // Default initialization
    std::string filePath;
    int volume{ MIX_MAX_VOLUME }; // Default volume (max)
};

class SoundService::Impl
{
public:
    Impl() : m_Stop(false)
    {
        Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);
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
        for (auto& pair : m_SoundCache) Mix_FreeChunk(pair.second);
        Mix_CloseAudio();
    }
    void Enqueue(SoundRequestType type, const std::string& file, int volume = MIX_MAX_VOLUME)
    {
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_Queue.push({ type, file, volume });
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
            case SoundRequestType::Load:
                if (m_SoundCache.find(req.filePath) == m_SoundCache.end())
                {
                    auto* chunk = Mix_LoadWAV(req.filePath.c_str());
                    if (chunk)
                    {
                        m_SoundCache[req.filePath] = chunk;
                        std::cout << "[DEBUG] Sound loaded successfully: " << req.filePath << std::endl;
                    }
                    else
                    {
                        std::cerr << "[ERROR] Failed to load sound: " << req.filePath
                            << " - " << Mix_GetError() << std::endl;
                    }
                }
                else
                {
                    std::cout << "[DEBUG] Sound already loaded: " << req.filePath << std::endl;
                }
                break;

            case SoundRequestType::Play:
                if (m_SoundCache.find(req.filePath) != m_SoundCache.end())
                {
                    Mix_VolumeChunk(m_SoundCache[req.filePath], req.volume); // Set volume
                    Mix_PlayChannel(-1, m_SoundCache[req.filePath], 0);
                    std::cout << "[DEBUG] Playing sound: " << req.filePath << " at volume: " << req.volume << std::endl;
                }
                else
                {
                    std::cerr << "[ERROR] Sound not loaded, cannot play: " << req.filePath << std::endl;
                }
                break;
            }
        }
    }

    std::unordered_map<std::string, Mix_Chunk*> m_SoundCache;
    std::queue<SoundRequest> m_Queue;
    std::mutex m_Mutex;
    std::condition_variable m_CondVar;
    std::thread m_Worker;
    std::atomic<bool> m_Stop;
};

SoundService::SoundService() : m_Impl(std::make_unique<Impl>()) {}
SoundService::~SoundService() = default;

void SoundService::LoadSound(const std::string& filePath)
{
    m_Impl->Enqueue(SoundRequestType::Load, filePath);
}

void SoundService::OutputSound(const std::string& filePath, int volume)
{
    m_Impl->Enqueue(SoundRequestType::Play, filePath, volume);
}
