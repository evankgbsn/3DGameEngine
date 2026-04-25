#include "Source.h"

#include "AudioManager.h"
#include "Sound.h"

Source::Source() :
    position(glm::vec3(0.0f)),
    rotation(glm::mat4(1.0f))
{
}

Source::Source(const std::string& n, const glm::vec3& pos, const glm::mat4& rot) :
    position(pos),
    rotation(rot),
    name(n)
{

}

Source::~Source()
{
}

Sound* Source::LoadSound(const std::string& name, const std::string& path)
{
    return AudioManager::LoadSound(name, path);
}

Sound* Source::GetSound(const std::string& name)
{
    return AudioManager::GetSound(name);
}

bool Source::SoundLoaded(const std::string& name)
{
    return AudioManager::SoundLoaded(name);
}

void Source::UnloadSound(const std::string& name)
{
    AudioManager::UnloadSound(name);
}

void Source::SetPosition(const glm::vec3& pos)
{
    position = pos;
}

glm::vec3 Source::GetPosition() const
{
    return position;
}

void Source::SetRotation(const glm::mat4& rot)
{
    rotation = rot;
}

glm::mat4 Source::GetRotation() const
{
    return rotation;
}

glm::vec3 Source::GetForward() const
{
    return -glm::normalize(glm::vec3(rotation[2]));
}

glm::vec3 Source::GetRight() const
{
    return glm::normalize(glm::vec3(rotation[0]));
}

glm::vec3 Source::GetUp() const
{
    return glm::normalize(glm::vec3(rotation[1]));
}

void Source::AddSound(const std::string& sound)
{
    sounds.insert(sound);

    // Get the base ma_node from the AudioManager
    // You'll need to make a helper: ma_node* GetSteamAudioNodeBase(string name)
    ma_node* pNode = AudioManager::GetSteamAudioNodeBase(name);

    Sound* soundPtr = AudioManager::GetSound(sound);

    if (pNode && soundPtr != nullptr && soundPtr->GetMiniAudioSound())
    {
        // 1. Detach from engine output
        ma_node_detach_output_bus(soundPtr->GetMiniAudioSound(), 0);

        // 2. Attach to our Steam Audio Node
        ma_node_attach_output_bus(soundPtr->GetMiniAudioSound(), 0, pNode, 0);
    }
}

void Source::RemoveSound(const std::string& sound)
{
    if (sounds.contains(sound))
    {
        Stop(sound);
        sounds.erase(sounds.find(sound));
    }
}

const std::set<std::string>& Source::GetSounds() const
{
    return sounds;
}

void Source::ClearSounds()
{
    sounds.clear();
}

void Source::Play(const std::string& sound)
{
    if (!sounds.contains(sound))
    {
        return;
    }

    Sound* soundPtr = AudioManager::GetSound(sound);

    if (soundPtr && soundPtr->GetMiniAudioSound())
    {
        ma_sound_start(soundPtr->GetMiniAudioSound());
    }
}

void Source::Stop(const std::string& sound)
{
    if (!sounds.contains(sound))
    {
        return;
    }

    Sound* soundPtr = AudioManager::GetSound(sound);

    if (soundPtr && soundPtr->GetMiniAudioSound())
    {
        ma_sound_stop(soundPtr->GetMiniAudioSound());
    }
}

void Source::PlayAll()
{
    for (const auto& sound : sounds)
    {
        Play(sound);
    }
}

void Source::StopAll()
{
    for (const auto& sound : sounds)
    {
        Stop(sound);
    }
}
