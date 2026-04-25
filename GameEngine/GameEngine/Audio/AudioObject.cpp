#include "AudioObject.h"

#include "../Renderer/Model/Model.h"
#include "AudioManager.h"

#include <phonon.h>

AudioObject::AudioObject(Model* const m, const glm::vec3& pos, const glm::mat4& rot, const Material& material) :
    mat(material),
    model(m),
    position(pos),
    rotation(rot),
    steamSubscene(nullptr)
{
    for (Vertex& vert : model->GetVertices())
    {
        vertices.push_back({ vert.GetPosition().x, vert.GetPosition().y, vert.GetPosition().z });
    }

    for (int i : model->GetIndices())
    {
        indices.push_back(i);
    }
    steamStaticMeshSettings.numVertices = vertices.size();
    steamStaticMeshSettings.numTriangles = indices.size() / 3;
    steamStaticMeshSettings.vertices = vertices.data();
    steamStaticMeshSettings.triangles = (IPLTriangle*)indices.data();
    steamStaticMeshSettings.materialIndices = NULL; // Use a single material for the whole mesh

    steamMaterial.absorption[0] = material.lowFrequencyAbsorption;
    steamMaterial.absorption[1] = material.midFrequencyAbsorption;
    steamMaterial.absorption[1] = material.highFrequencyAbsorption;

    steamMaterial.scattering = material.scattering;

    steamMaterial.transmission[0] = material.lowFrequencyTransmission;
    steamMaterial.transmission[1] = material.midFrequencyTransmission;
    steamMaterial.transmission[1] = material.highFrequencyTransmission;

    steamStaticMeshSettings.numMaterials = 1;
    steamStaticMeshSettings.materials = &steamMaterial;

    IPLSceneSettings subSceneSettings{};

    // Use IPL_SCENETYPE_DEFAULT or IPL_SCENETYPE_EMBREE
    // Note: Embree is generally faster but requires the Embree DLL/library.
    subSceneSettings.type = IPL_SCENETYPE_DEFAULT;

    // If you are using the Default raytracer, these can be left as null
    subSceneSettings.closestHitCallback = nullptr;
    subSceneSettings.anyHitCallback = nullptr;
    subSceneSettings.batchedClosestHitCallback = nullptr;
    subSceneSettings.batchedAnyHitCallback = nullptr;
    subSceneSettings.userData = nullptr;

    // If you chose Embree, you may need to provide the Embree device
    subSceneSettings.embreeDevice = nullptr; // Steam Audio can often manage this internally

    // 1. ASSET LOAD TIME
    iplSceneCreate(AudioManager::GetSteamAudioContext(), &subSceneSettings, steamSubscene);

    IPLStaticMesh staticMesh;
    // ... fill meshSettings ...
    iplStaticMeshCreate(*steamSubscene, &steamStaticMeshSettings, &staticMesh);

    iplSceneCommit(*steamSubscene); // <--- MANDATORY BEFORE INSTANCING
}

AudioObject::~AudioObject()
{
}

void AudioObject::SetPosition(const glm::vec3& pos)
{
    position = pos;
    AudioManager::UpdateObjectTransform(this);
}

glm::vec3 AudioObject::GetPosition() const
{
    return position;
}

void AudioObject::SetRotation(const glm::mat4& rot)
{
    rotation = rot;
    AudioManager::UpdateObjectTransform(this);
}

glm::mat4 AudioObject::GetRotation() const
{
    return rotation;
}

void AudioObject::SetModel(Model* const m)
{
    model = m;

    for (Vertex& vert : model->GetVertices())
    {
        vertices.push_back({ vert.GetPosition().x, vert.GetPosition().y, vert.GetPosition().z });
    }

    for (int i : model->GetIndices())
    {
        indices.push_back(i);
    }
    steamStaticMeshSettings.numVertices = vertices.size();
    steamStaticMeshSettings.numTriangles = indices.size() / 3;
    steamStaticMeshSettings.vertices = vertices.data();
    steamStaticMeshSettings.triangles = (IPLTriangle*)indices.data();
    steamStaticMeshSettings.materialIndices = NULL; // Use a single material for the whole mesh

}

Model* const AudioObject::GetModel() const
{
    return model;
}

void AudioObject::UpdateMaterial(const Material& newMat)
{
    mat = newMat;

    steamMaterial.absorption[0] = mat.lowFrequencyAbsorption;
    steamMaterial.absorption[1] = mat.midFrequencyAbsorption;
    steamMaterial.absorption[1] = mat.highFrequencyAbsorption;

    steamMaterial.scattering = mat.scattering;

    steamMaterial.transmission[0] = mat.lowFrequencyTransmission;
    steamMaterial.transmission[1] = mat.midFrequencyTransmission;
    steamMaterial.transmission[1] = mat.highFrequencyTransmission;

    steamStaticMeshSettings.numMaterials = 1;
    steamStaticMeshSettings.materials = &steamMaterial;
}

const AudioObject::Material& AudioObject::GetMaterial() const
{
    return mat;
}
