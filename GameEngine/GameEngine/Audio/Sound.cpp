#include "Sound.h"
#include "AudioManager.h"
#include "miniaudio.h"
#include "../Utils/Logger.h" 

Sound::Sound(const std::string& n, const std::string& p) :
    name(n),
    path(p),
    sound(new ma_sound())
{
}

Sound::~Sound()
{
    delete sound;
}

void Sound::Load()
{
    ma_sound_config soundConfig = ma_sound_config_init();
    soundConfig.pFilePath = path.c_str();

    // MA_SOUND_FLAG_NO_DEFAULT_ATTACHMENT prevents miniaudio from automatically 
    // plugging this Mono sound into the Stereo engine output, avoiding Error -3.
    soundConfig.flags = MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_NO_DEFAULT_ATTACHMENT;
    soundConfig.channelsOut = 1;

    ma_result result = ma_sound_init_ex(AudioManager::GetMiniAudioEngine(), &soundConfig, sound);

    if (result != MA_SUCCESS)
    {
        Logger::Log(std::string("Failed to load sound: ") + name + " from path: " + path + " (Error: " + std::to_string(result) + ")", Logger::Category::Error);
    }
    else
    {
        ma_sound_set_volume(sound, 1.0f);
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

ma_sound* const Sound::GetMiniAudioSound() const
{
    return sound;
}