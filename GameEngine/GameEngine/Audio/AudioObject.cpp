#include "AudioObject.h"

#include "../Renderer/Model/Model.h"
#include "AudioManager.h"

#include <phonon.h>

AudioObject::AudioObject(const Model* const m, const glm::vec3& pos, const glm::mat4& rot, const Material& material) :
    mat(material),
    model(m),
    position(pos),
    rotation(rot)
{
    for (const Vertex& vert : model->GetVertices())
    {
        vertices.push_back({ vert.GetPosition().x, vert.GetPosition().y, vert.GetPosition().z });
    }

    for (unsigned int i = 0; i < model->GetIndices().size(); i += 3)
    {
        IPLTriangle newTri = { model->GetIndices()[0 + i],
                model->GetIndices()[1 + i],
                model->GetIndices()[2 + i] };

        triangles.push_back(newTri);
        materialIndices.push_back(0);
    }


    steamStaticMeshSettings.numVertices = vertices.size();
    steamStaticMeshSettings.numTriangles = triangles.size();
    steamStaticMeshSettings.vertices = vertices.data();
    steamStaticMeshSettings.triangles = triangles.data();
    steamStaticMeshSettings.materialIndices = materialIndices.data(); // Use a single material for the whole mesh

    steamMaterial.absorption[0] = material.lowFrequencyAbsorption;
    steamMaterial.absorption[1] = material.midFrequencyAbsorption;
    steamMaterial.absorption[2] = material.highFrequencyAbsorption;

    steamMaterial.scattering = material.scattering;

    steamMaterial.transmission[0] = material.lowFrequencyTransmission;
    steamMaterial.transmission[1] = material.midFrequencyTransmission;
    steamMaterial.transmission[2] = material.highFrequencyTransmission;

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
    iplSceneCreate(AudioManager::GetSteamAudioContext(), &subSceneSettings, &steamSubscene);

    // ... fill meshSettings ...
    iplStaticMeshCreate(steamSubscene, &steamStaticMeshSettings, &steamStaticMesh);

    iplSceneCommit(steamSubscene); // <--- MANDATORY BEFORE INSTANCING
}

AudioObject::~AudioObject()
{
    // Release the mesh handle first
    // Note: You need to store 'staticMesh' as a member in the header to do this
    if (steamStaticMesh) {
        iplStaticMeshRelease(&steamStaticMesh);
    }

    // Release the subscene handle
    if (steamSubscene) {
        iplSceneRelease(&steamSubscene);
    }

    iplSceneRelease(&steamSubscene);
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

const Model* const AudioObject::GetModel() const
{
    return model;
}

void AudioObject::UpdateMaterial(const Material& newMat)
{
    mat = newMat;

    steamMaterial.absorption[0] = mat.lowFrequencyAbsorption;
    steamMaterial.absorption[1] = mat.midFrequencyAbsorption;
    steamMaterial.absorption[2] = mat.highFrequencyAbsorption;

    steamMaterial.scattering = mat.scattering;

    steamMaterial.transmission[0] = mat.lowFrequencyTransmission;
    steamMaterial.transmission[1] = mat.midFrequencyTransmission;
    steamMaterial.transmission[2] = mat.highFrequencyTransmission;

    steamStaticMeshSettings.numMaterials = 1;
    steamStaticMeshSettings.materials = &steamMaterial;
}

const AudioObject::Material& AudioObject::GetMaterial() const
{
    return mat;
}

