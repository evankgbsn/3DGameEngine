#include "AudioSource.h"

#include "../../Audio/AudioManager.h"
#include "../../Audio/Source.h"

AudioSource::AudioSource(const glm::vec3& position, const glm::mat4& rotation) :
    source(nullptr)
{
    RegisterComponentClassType<AudioSource>(this);

    static unsigned long long ID = 0;
    source = AudioManager::CreateSource(std::to_string(ID++), position, rotation);
}

AudioSource::AudioSource() :
    source(nullptr)
{
    RegisterComponentClassType<AudioSource>(this);
}

AudioSource::~AudioSource()
{
    AudioManager::Delete(source);
}

Sound* AudioSource::LoadSound(const std::string& name, const std::string& path)
{
    return Source::LoadSound(name, path);
}

Sound* AudioSource::GetSound(const std::string& name)
{
    return Source::GetSound(name);
}

bool AudioSource::SoundLoaded(const std::string& name)
{
    return Source::SoundLoaded(name);
}

void AudioSource::UnloadSound(const std::string& name)
{
    Source::UnloadSound(name);
}

void AudioSource::SetPosition(const glm::vec3& pos)
{
    source->SetPosition(pos);
}

glm::vec3 AudioSource::GetPosition() const
{
    return source->GetPosition();
}

void AudioSource::SetRotation(const glm::mat4& rot)
{
    source->SetRotation(rot);
}

glm::mat4 AudioSource::GetRotation() const
{
    return source->GetRotation();
}

glm::vec3 AudioSource::GetForward() const
{
    return source->GetForward();
}

glm::vec3 AudioSource::GetRight() const
{
    return source->GetRight();
}

glm::vec3 AudioSource::GetUp() const
{
    return source->GetUp();
}

void AudioSource::AddSound(const std::string& sound)
{
    source->AddSound(sound);
}

void AudioSource::RemoveSound(const std::string& sound)
{
    source->RemoveSound(sound);
}

const std::set<std::string>& AudioSource::GetSounds() const
{
    return source->GetSounds();
}

void AudioSource::ClearSounds()
{
    source->ClearSounds();
}

void AudioSource::Play(const std::string& sound)
{
    source->Play(sound);
}

void AudioSource::Stop(const std::string& sound)
{
    source->Stop(sound);
}

void AudioSource::PlayAll()
{
    source->PlayAll();
}

void AudioSource::StopAll()
{
    source->StopAll();
}

void AudioSource::Serialize()
{
}

void AudioSource::Deserialize()
{
}

void AudioSource::Update()
{
}
