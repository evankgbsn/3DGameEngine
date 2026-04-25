#include "AudioManager.h"

#include "../Utils/Logger.h"

#include "miniaudio.h"

AudioManager* AudioManager::instance = nullptr;

void AudioManager::Initialize()
{
	SingletonHelpers::InitializeSingleton(&instance, "Initialized AudioManager");
}

void AudioManager::Terminate()
{
	SingletonHelpers::TerminateSingleton(&instance, "Terminated AudioManager");
}

void AudioManager::EditorUpdate()
{
}

void AudioManager::GameUpdate()
{
}

ma_engine* AudioManager::GetMiniAudioEngine()
{
	ma_engine* ret = nullptr;

	if (instance != nullptr)
	{
		ret = instance->miniAudioEngine;
	}
	else
	{
		Logger::Log(std::string("Calling AudioManager::GetMiniAudioEngine before AudioManager::Initialize"), Logger::Category::Warning);
	}

	return ret;
}

AudioManager::AudioManager() :
	miniAudioEngine(nullptr)
{
	InitializeMiniAudio();
}

AudioManager::~AudioManager()
{
	delete miniAudioEngine;
}

void AudioManager::InitializeMiniAudio()
{
	miniAudioEngine = new ma_engine();

	ma_result result = ma_engine_init(nullptr, miniAudioEngine);

	if (result != MA_SUCCESS)
	{
		Logger::Log(std::string("Failed to initialize MiniAudioEngine"), Logger::Category::Error);
	}
}
