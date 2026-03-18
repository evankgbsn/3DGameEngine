#include "Sound.h"

#include "AudioManager.h"

#include "miniaudio.h"

Sound::Sound(const std::string& name, const std::string& path) :
    sound(new ma_sound())
{
}

Sound::~Sound()
{
    delete sound;
}

void Sound::Load()
{
    ma_result result = ma_sound_init_from_file(AudioManager::GetMiniAudioEngine(), path.c_str(), 0, nullptr, nullptr, sound);

    if (result != MA_SUCCESS)
    {
        Logger::Log(std::string("Failed to load sound: ") + name + " from path: " + path, Logger::Category::Error);
    }
    else
    {
        Logger::Log(std::string("Loaded sound: ") + name + " from path: " + path, Logger::Category::Success);
    }
}

void Sound::Unload()
{
    ma_sound_uninit(sound);
    Logger::Log(std::string("Unloaded sound: ") + name + " from path : " + path, Logger::Category::Success);
}

const std::string& Sound::GetName() const
{
    return name;
}

const std::string& Sound::GetPath() const
{
    return path;
}
