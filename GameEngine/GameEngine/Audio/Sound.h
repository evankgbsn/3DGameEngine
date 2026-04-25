#ifndef SOUND_H
#define SOUND_H

#include <string>

struct ma_sound;

class Sound
{
public:

	Sound(const std::string& name, const std::string& path);

	~Sound();

	void Load();

	void Unload();

	const std::string& GetName() const;

	const std::string& GetPath() const;

	ma_sound* const GetMiniAudioSound() const;

private:

	friend class AudioManager;

	Sound() = delete;

	Sound(const Sound&) = delete;

	Sound& operator=(const Sound&) = delete;

	Sound(Sound&&) = delete;

	Sound& operator=(Sound&&) = delete;

	std::string name;

	std::string path;

	ma_sound* sound;
};

#endif // SOUND_H