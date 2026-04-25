#include "AudioOcclusionVolume.h"

#include "../../Audio/AudioObject.h"
#include "../../Audio/AudioManager.h"
#include "../../Renderer/Model/ModelManager.h"

AudioOcclusionVolume::AudioOcclusionVolume() :
    obj(nullptr)
{
    RegisterComponentClassType<AudioOcclusionVolume>(this);
}

AudioOcclusionVolume::AudioOcclusionVolume(const std::string& modelName, const glm::vec3& position, const glm::mat4& rotation, const Material& material) :
    obj(nullptr)
{
    RegisterComponentClassType<AudioOcclusionVolume>(this);

    static unsigned long long ID = 0;
    obj = AudioManager::CreateAudioObject(std::to_string(ID++), ModelManager::GetModel(modelName), position, rotation,
        {
            material.lowFrequencyAbsorption,
            material.midFrequencyAbsorption,
            material.highFrequencyAbsorption,
            material.scattering,
            material.lowFrequencyTransmission,
            material.midFrequencyTransmission,
            material.highFrequencyTransmission
        });
}

AudioOcclusionVolume::~AudioOcclusionVolume()
{
    AudioManager::Delete(obj);
}

void AudioOcclusionVolume::SetPosition(const glm::vec3& position)
{
    obj->SetPosition(position);
}

glm::vec3 AudioOcclusionVolume::GetPosition() const
{
    return obj->GetPosition();
}

void AudioOcclusionVolume::SetRotation(const glm::mat4& rotation)
{
    obj->SetRotation(rotation);
}

glm::mat4 AudioOcclusionVolume::GetRotation() const
{
    return obj->GetRotation();
}

void AudioOcclusionVolume::Serialize()
{
}

void AudioOcclusionVolume::Deserialize()
{
}

void AudioOcclusionVolume::Update()
{
}
