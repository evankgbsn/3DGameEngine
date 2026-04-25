#include "AudioListener.h"

#include "../../Audio/AudioManager.h"
#include "../../Audio/Listener.h"

AudioListener::AudioListener(const glm::vec3& position, const glm::mat4& rotation) :
    listener(nullptr)
{
    RegisterComponentClassType<AudioListener>(this);

    unsigned long long ID = 0;
    listener = AudioManager::CreateListener(stringID = std::to_string(ID++), position, rotation);
}

AudioListener::AudioListener() :
    listener(nullptr)
{
    RegisterComponentClassType<AudioListener>(this);
}

AudioListener::~AudioListener()
{
    AudioManager::Delete(listener);
}

void AudioListener::SetPosition(const glm::vec3& pos)
{
    listener->SetPosition(pos);
}

glm::vec3 AudioListener::GetPosition() const
{
    return listener->GetPosition();
}

void AudioListener::SetRotation(const glm::mat4& rot)
{
    listener->SetRotation(rot);
}

glm::mat4 AudioListener::GetRotation() const
{
    return listener->GetRotation();
}

glm::vec3 AudioListener::GetForward() const
{
    return listener->GetForward();
}

glm::vec3 AudioListener::GetRight() const
{
    return listener->GetRight();
}

glm::vec3 AudioListener::GetUp() const
{
    return listener->GetUp();
}

void AudioListener::SetActive() const
{
    AudioManager::SetActiveListener(stringID);
}

void AudioListener::Serialize()
{
}

void AudioListener::Deserialize()
{
}

void AudioListener::Update()
{
}
