#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include "../Utils/SingletonHelpers.h"

struct ma_engine;

class AudioManager
{
public:

	static void Initialize();

	static void Terminate();

	static void EditorUpdate();

	static void GameUpdate();

	static ma_engine* GetMiniAudioEngine();

private:

	friend class SingletonHelpers;

	AudioManager();

	~AudioManager();

	AudioManager(const AudioManager&) = delete;

	AudioManager& operator=(const AudioManager&) = delete;

	AudioManager(AudioManager&&) = delete;

	AudioManager& operator=(AudioManager&&) = delete;

	void InitializeMiniAudio();

	static AudioManager* instance;

	ma_engine* miniAudioEngine;

};

#endif // AUDIOMANAGER_H