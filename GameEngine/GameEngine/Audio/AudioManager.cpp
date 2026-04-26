#include "AudioManager.h"
#include "../Utils/Logger.h"
#include "AudioObject.h"
#include "Listener.h"
#include "Source.h"
#include "Sound.h"
#include <cmath> // Added for std::abs

static ma_node_vtable g_SteamAudioNodeVTable = {
    AudioManager::SteamAudioNodeProcessPCMFrames,
    nullptr,
    1,
    1,
    MA_NODE_FLAG_CONTINUOUS_PROCESSING
};

AudioManager* AudioManager::instance = nullptr;

void AudioManager::Initialize()
{
    SingletonHelpers::InitializeSingleton(&instance, "AudioManager");
}

void AudioManager::Terminate()
{
    instance->ClearAll();
    SingletonHelpers::TerminateSingleton(&instance, "AudioManager");
}

void AudioManager::EditorUpdate()
{
}

void AudioManager::UpdateListener()
{
    glm::vec3 listenerPos = listeners[activeListener]->GetPosition();
    glm::vec3 listenerForward = listeners[activeListener]->GetForward();
    glm::vec3 listenerRight = listeners[activeListener]->GetRight();
    glm::vec3 listenerUp = listeners[activeListener]->GetUp();

    IPLSimulationSharedInputs sharedInputs = { 0 };
    sharedInputs.listener.origin = { listenerPos.x, listenerPos.y, listenerPos.z };
    sharedInputs.listener.ahead = { listenerForward.x, listenerForward.y, listenerForward.z };
    sharedInputs.listener.up = { listenerUp.x, listenerUp.y, listenerUp.z };
    sharedInputs.listener.right = { listenerRight.x, listenerRight.y, listenerRight.z };

    iplSimulatorSetSharedInputs(simulator, IPL_SIMULATIONFLAGS_DIRECT, &sharedInputs);
}

void AudioManager::UpdateSources()
{
    for (const auto& source : sources)
    {
        IPLSimulationInputs inputs = {};
        inputs.flags = IPL_SIMULATIONFLAGS_DIRECT;
        inputs.directFlags = static_cast<IPLDirectSimulationFlags>(
            IPL_DIRECTSIMULATIONFLAGS_OCCLUSION |
            IPL_DIRECTSIMULATIONFLAGS_TRANSMISSION |
            IPL_DIRECTSIMULATIONFLAGS_AIRABSORPTION |
            IPL_DIRECTSIMULATIONFLAGS_DISTANCEATTENUATION
            );

        inputs.source.origin = { source.second->GetPosition().x, source.second->GetPosition().y, source.second->GetPosition().z };
        inputs.source.ahead = { source.second->GetForward().x, source.second->GetForward().y, source.second->GetForward().z };

        inputs.occlusionType = IPL_OCCLUSIONTYPE_VOLUMETRIC;
        inputs.numOcclusionSamples = 64;

        // FIX: If the sound is blocked, this tells the simulator to fire a ray 
        // to find the material's 'transmission' value (muffling).
        inputs.occlusionRadius = 1.2f;

        inputs.numTransmissionRays = 64;

        // Ensure distance model is set so the simulator stays 'awake' for this source
        inputs.distanceAttenuationModel.type = IPL_DISTANCEATTENUATIONTYPE_DEFAULT;
        inputs.distanceAttenuationModel.minDistance = 1.0f;

        IPLSimulationSharedInputs sharedInputs = {};
        sharedInputs.listener = listeners[activeListener]->GetIPLCoordinateSystem(); // Ensure this returns an IPLCoordinateSystem
        iplSimulatorSetSharedInputs(simulator, IPL_SIMULATIONFLAGS_DIRECT, &sharedInputs);

        iplSourceSetInputs(steamSources[source.first], IPL_SIMULATIONFLAGS_DIRECT, &inputs);
    }
}

void AudioManager::ProccessUpdatedSounds()
{
    glm::vec3 L = instance->listeners[instance->activeListener]->GetPosition();
    glm::mat4 listenerInvRot = glm::inverse(instance->listeners[instance->activeListener]->GetRotation());

    for (auto& sourcePair : instance->sources)
    {
        const std::string& name = sourcePair.first;
        Source* src = sourcePair.second;

        IPLSimulationOutputs outputs = { };
        iplSourceGetOutputs(instance->steamSources[name], IPL_SIMULATIONFLAGS_DIRECT, &outputs);

        glm::vec3 S = src->GetPosition();
        glm::vec3 diff = S - L;
        glm::vec3 localDir = glm::vec3(listenerInvRot * glm::vec4(diff, 0.0f));

        float distance = glm::length(localDir);
        glm::vec3 relativeDir(0.0f, 0.0f, 1.0f);
        if (distance > 0.001f)
        {
            relativeDir = localDir / distance;
        }

        // If the simulator is working, it will now return a value < 1.0 here
        float distAtten = outputs.direct.distanceAttenuation;

        // Safety Fallback (Keep this for now to ensure volume isn't 0)
        if (distAtten >= 0.999f && distance > 1.1f) {
            distAtten = 1.0f / distance;
        }

        if (steamAudioNodes.count(name)) {
            steamAudioNodes[name].directParams.occlusion = outputs.direct.occlusion;
            steamAudioNodes[name].directParams.transmission[0] = outputs.direct.transmission[0];
            steamAudioNodes[name].directParams.transmission[1] = outputs.direct.transmission[1];
            steamAudioNodes[name].directParams.transmission[2] = outputs.direct.transmission[2];
            steamAudioNodes[name].directParams.distanceAttenuation = distAtten;
            steamAudioNodes[name].direction = { relativeDir.x, relativeDir.y, relativeDir.z };
        }
    }
}

void AudioManager::GameUpdate()
{
    if (instance != nullptr)
    {
        instance->UpdateListener();
        instance->UpdateSources();

        iplSimulatorRunDirect(instance->simulator);

        instance->ProccessUpdatedSounds();
    }
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

AudioObject* AudioManager::CreateAudioObject(const std::string& name, const Model* const model, const glm::vec3& position, const glm::mat4& rotation, const AudioObject::Material& material)
{
    AudioObject* ret = nullptr;
    if (instance != nullptr)
    {
        if (instance->objects.find(name) == instance->objects.end())
        {
            ret = instance->objects[name] = new AudioObject(model, position, rotation, material);

            if (instance->scene) {
                IPLInstancedMeshSettings instSettings = { 0 };
                instSettings.subScene = ret->steamSubscene;
                instSettings.transform = ConvertGlmToIpl(ret->GetPosition(), ret->GetRotation());

                iplInstancedMeshCreate(instance->scene, &instSettings, &instance->steamStaticMeshes[name]);

                // FIX: Add dynamically spawned meshes to the main scene!
                iplInstancedMeshAdd(instance->steamStaticMeshes[name], instance->scene);

                // DEBUG: Track the hierarchy update
                iplSceneCommit(instance->scene);
                iplSimulatorSetScene(instance->simulator, instance->scene);
                iplSimulatorCommit(instance->simulator);
            }
        }
        else
        {
            Logger::Log("Audio object with the name " + name + " already exsists.", Logger::Category::Warning);
            ret = instance->objects[name];
        }
    }
    return ret;
}

AudioObject* AudioManager::GetAudioObject(const std::string& name)
{
    if (instance != nullptr && instance->objects.find(name) != instance->objects.end())
    {
        return instance->objects[name];
    }
    return nullptr;
}

Listener* AudioManager::CreateListener(const std::string& name, const glm::vec3& position, const glm::mat4& rotation)
{
    Listener* ret = nullptr;
    if (instance != nullptr)
    {
        if (instance->listeners.find(name) == instance->listeners.end())
        {
            ret = instance->listeners[name] = new Listener(position, rotation);
        }
        else
        {
            ret = instance->listeners[name];
        }
    }
    return ret;
}

Listener* AudioManager::GetListener(const std::string& name)
{
    if (instance != nullptr && instance->listeners.find(name) != instance->listeners.end())
    {
        return instance->listeners[name];
    }
    return nullptr;
}

Source* AudioManager::CreateSource(const std::string& name, const glm::vec3& position, const glm::mat4& rotation)
{
    Source* ret = nullptr;
    if (instance != nullptr)
    {
        if (instance->sources.find(name) == instance->sources.end())
        {
            ret = instance->sources[name] = new Source(name, position, rotation);

            IPLSourceSettings sourceSettings = { };
            sourceSettings.flags = IPL_SIMULATIONFLAGS_DIRECT;
            iplSourceCreate(instance->simulator, &sourceSettings, &instance->steamSources[name]);

            // FIX: Explicitly add the newly created source to the raycast simulator!
            iplSourceAdd(instance->steamSources[name], instance->simulator);
            iplSimulatorCommit(instance->simulator);

            instance->InitializeSteamAudioNode(name);
        }
        else
        {
            ret = instance->sources[name];
        }
    }
    return ret;
}

Source* AudioManager::GetSource(const std::string& name)
{
    if (instance != nullptr && instance->sources.find(name) != instance->sources.end())
    {
        return instance->sources[name];
    }
    return nullptr;
}

Sound* AudioManager::LoadSound(const std::string& name, const std::string& path)
{
    Sound* ret = nullptr;
    if (instance != nullptr)
    {
        if (instance->sounds.find(name) == instance->sounds.end())
        {
            instance->sounds[name] = ret = new Sound(name, path);
            ret->Load();
        }
        else
        {
            ret = instance->sounds[name];
        }
    }
    return ret;
}

Sound* AudioManager::GetSound(const std::string& name)
{
    if (instance != nullptr && instance->sounds.find(name) != instance->sounds.end())
    {
        return instance->sounds[name];
    }
    return nullptr;
}

bool AudioManager::SoundLoaded(const std::string& name)
{
    if (instance != nullptr)
    {
        return instance->sounds.find(name) != instance->sounds.end();
    }
    return false;
}

void AudioManager::UnloadSound(const std::string& name)
{
    if (instance != nullptr && instance->sounds.find(name) != instance->sounds.end())
    {
        instance->sounds[name]->Unload();
        delete instance->sounds[name];
        instance->sounds.erase(name);
    }
}

void AudioManager::Delete(Listener* const l)
{
    if (instance != nullptr)
    {
        for (auto& listener : instance->listeners)
        {
            if (listener.second == l)
            {
                if (listener.first == instance->activeListener)
                {
                    Logger::Log("Attempting to delete active listener.", Logger::Category::Error);
                    return;
                }
                delete l;
                instance->listeners.erase(listener.first);
                return;
            }
        }
    }
}

void AudioManager::Delete(AudioObject* const obj)
{
    if (instance != nullptr)
    {
        for (auto it = instance->objects.begin(); it != instance->objects.end(); ++it)
        {
            if (it->second == obj)
            {
                const std::string& name = it->first;

                if (instance->steamStaticMeshes.count(name)) {
                    // FIX: Remove the mesh from the scene before releasing memory
                    iplInstancedMeshRemove(instance->steamStaticMeshes[name], instance->scene);
                    iplInstancedMeshRelease(&instance->steamStaticMeshes[name]);
                    instance->steamStaticMeshes.erase(name);
                }

                delete obj;

                // FIX: Commit both the Scene and the Simulator to rebuild the raycast BVH
                iplSceneCommit(instance->scene);
                iplSimulatorCommit(instance->simulator);

                instance->objects.erase(it);
                return;
            }
        }
    }
}

void AudioManager::Delete(Source* const source)
{
    if (instance != nullptr)
    {
        for (auto& src : instance->sources)
        {
            if (src.second == source)
            {
                instance->CleanupSource(src.first);
                delete source;
                instance->sources.erase(src.first);
                return;
            }
        }
    }
}

void AudioManager::Delete(const std::string& name)
{
    if (instance == nullptr) return;

    bool sceneChanged = false;

    if (instance->objects.count(name))
    {
        if (instance->steamStaticMeshes.count(name))
        {
            // FIX: Remove the mesh from the physics engine before destroying it to prevent Access Violations
            iplInstancedMeshRemove(instance->steamStaticMeshes[name], instance->scene);
            iplInstancedMeshRelease(&instance->steamStaticMeshes[name]);
            instance->steamStaticMeshes.erase(name);
            sceneChanged = true;
        }

        delete instance->objects[name];
        instance->objects.erase(name);
    }

    if (instance->sources.count(name))
    {
        instance->CleanupSource(name);
        delete instance->sources[name];
        instance->sources.erase(name);
    }

    if (instance->listeners.count(name))
    {
        if (name == instance->activeListener)
        {
            Logger::Log("Cannot delete the active listener: " + name, Logger::Category::Error);
        }
        else
        {
            delete instance->listeners[name];
            instance->listeners.erase(name);
        }
    }

    if (sceneChanged)
    {
        iplSceneCommit(instance->scene);
        iplSimulatorCommit(instance->simulator); // FIX: Committing the simulator ensures the BVH is rebuilt without the destroyed wall
    }
}

AudioManager::AudioManager() :
    miniAudioEngine(nullptr)
{
    InitializeSteamAudio();
    InitializeMiniAudio();
}

AudioManager::~AudioManager()
{
    for (const auto& source : sources)
    {
        CleanupSource(source.first);
    }

    iplSimulatorRelease(&simulator);
    iplSceneRelease(&scene);
    iplHRTFRelease(&hrtf);
    iplContextRelease(&context);

    ma_engine_uninit(miniAudioEngine);
    delete miniAudioEngine;

    for (const auto& sound : sounds)
    {
        delete sound.second;
    }
}

void AudioManager::InitializeMiniAudio()
{
    miniAudioEngine = new ma_engine();

    ma_engine_config engineConfig = ma_engine_config_init();
    engineConfig.sampleRate = 48000;
    engineConfig.periodSizeInFrames = 1024;

    ma_result result = ma_engine_init(&engineConfig, miniAudioEngine);

    if (result != MA_SUCCESS)
    {
        Logger::Log(std::string("Failed to initialize MiniAudioEngine"), Logger::Category::Error);
    }
    else
    {
        Logger::Log("MiniAudioEngine Initialized. Sample Rate: " + std::to_string(ma_engine_get_sample_rate(miniAudioEngine)), Logger::Category::Success);
    }

    ma_engine_set_volume(miniAudioEngine, 1.0f);
}

void AudioManager::InitializeSteamAudio()
{
    contextSettings = { 0 };
    contextSettings.version = STEAMAUDIO_VERSION;

    iplContextCreate(&contextSettings, &context);

    iplSettings.samplingRate = 48000;
    iplSettings.frameSize = 1024;

    hrtfSettings.type = IPL_HRTFTYPE_DEFAULT;
    hrtfSettings.sofaFileName = nullptr;
    hrtfSettings.volume = 1.0f;

    iplHRTFCreate(context, &iplSettings, &hrtfSettings, &hrtf);

    simulatorSettings = {};
    simulatorSettings.maxNumSources = 64;
    simulatorSettings.numThreads = 2;
    simulatorSettings.maxNumOcclusionSamples = 64;

    // FIX: You MUST tell the simulator to enable Direct simulation (Occlusion/Transmission/Distance)
    simulatorSettings.flags = static_cast<IPLSimulationFlags>(
        IPL_SIMULATIONFLAGS_DIRECT
        );

    iplSimulatorCreate(context, &simulatorSettings, &simulator);
}

void AudioManager::InitializeSteamAudioScene()
{
    if (instance != nullptr)
    {

        for (auto& meshPair : instance->steamStaticMeshes) {
            iplInstancedMeshRemove(meshPair.second, instance->scene);
            iplInstancedMeshRelease(&meshPair.second);
        }
        instance->steamStaticMeshes.clear();

        if (instance->scene) {
            iplSceneRelease(&instance->scene);
            instance->scene = nullptr;
        }

        IPLSceneSettings sceneSettings = { IPL_SCENETYPE_DEFAULT };
        iplSceneCreate(AudioManager::GetSteamAudioContext(), &sceneSettings, &instance->scene);

        int objectCounter = 0;
        for (auto& object : instance->objects)
        {
            IPLInstancedMeshSettings instSettings = { 0 };
            instSettings.subScene = object.second->steamSubscene;
            instSettings.transform = ConvertGlmToIpl(object.second->GetPosition(), object.second->GetRotation());

            iplInstancedMeshCreate(instance->scene, &instSettings, &instance->steamStaticMeshes[object.first]);
            iplInstancedMeshAdd(instance->steamStaticMeshes[object.first], instance->scene);
        }

        iplSceneCommit(instance->scene);
        iplSimulatorSetScene(instance->simulator, nullptr);
        iplSimulatorSetScene(instance->simulator, instance->scene);
        iplSimulatorCommit(instance->simulator);
    }
}

void AudioManager::SetActiveListener(const std::string& name)
{
    if (instance != nullptr && instance->listeners.find(name) != instance->listeners.end())
    {
        instance->activeListener = name;
    }
}

void AudioManager::SteamAudioNodeProcessPCMFrames(ma_node* pNode, const float** ppFramesIn, ma_uint32* pFrameCountIn, float** ppFramesOut, ma_uint32* pFrameCountOut)
{
    SteamAudioNode* pSteamNode = (SteamAudioNode*)pNode;

    ma_uint32 framesToProcess = *pFrameCountIn > 1024 ? 1024 : *pFrameCountIn;

    memset(pSteamNode->inputPlane, 0, 1024 * sizeof(float));
    memcpy(pSteamNode->inputPlane, ppFramesIn[0], framesToProcess * sizeof(float));

    IPLAudioBuffer inBuffer;
    inBuffer.numChannels = 1;
    inBuffer.numSamples = framesToProcess;
    inBuffer.data = &pSteamNode->inputPlane;

    IPLAudioBuffer midBuffer;
    midBuffer.numChannels = 1;
    midBuffer.numSamples = framesToProcess;
    midBuffer.data = &pSteamNode->midPlane;

    IPLAudioBuffer outBuffer;
    outBuffer.numChannels = 2;
    outBuffer.numSamples = framesToProcess;
    outBuffer.data = pSteamNode->outputPlanes;

    // 1. Direct Effect (Occlusion / Distance) -> Writes from inBuffer to midBuffer
    iplDirectEffectApply(pSteamNode->directEffect, &pSteamNode->directParams, &inBuffer, &midBuffer);

    // 2. Binaural Effect (Spatialization) -> Writes from midBuffer to outBuffer (outputPlanes)
    IPLBinauralEffectParams binParams = { 0 };

    // MATH SAFETY: Ensure the direction vector is a valid Unit Vector to prevent NaN explosions
    float dirLen = std::sqrt(pSteamNode->direction.x * pSteamNode->direction.x +
        pSteamNode->direction.y * pSteamNode->direction.y +
        pSteamNode->direction.z * pSteamNode->direction.z);

    if (dirLen < 0.001f) {
        binParams.direction = { 0.0f, 0.0f, 1.0f }; // Safe fallback
    }
    else {
        binParams.direction = {
            pSteamNode->direction.x / dirLen,
            pSteamNode->direction.y / dirLen,
            pSteamNode->direction.z / dirLen
        };
    }

    binParams.hrtf = pSteamNode->hrtf;
    binParams.interpolation = IPL_HRTFINTERPOLATION_BILINEAR;
    binParams.spatialBlend = 1.0f;

    iplBinauralEffectApply(pSteamNode->binauralEffect, &binParams, &midBuffer, &outBuffer);

    // 3. Hand back to Miniaudio
    ma_interleave_pcm_frames(
        ma_format_f32,
        2,
        framesToProcess,
        (const void**)pSteamNode->outputPlanes,
        ppFramesOut[0]
    );

    *pFrameCountOut = framesToProcess;
}

const IPLContext& AudioManager::GetSteamAudioContext()
{
    static IPLContext defaultReturn;
    if (instance != nullptr) return instance->context;
    return defaultReturn;
}

IPLMatrix4x4 AudioManager::ConvertGlmToIpl(const glm::vec3& position, const glm::mat4& rotation) {
    glm::mat4 model = rotation;
    model[3][0] = position.x;
    model[3][1] = position.y;
    model[3][2] = position.z;
    model[3][3] = 1.0f;

    IPLMatrix4x4 iplMat;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            iplMat.elements[i][j] = model[j][i];
        }
    }
    return iplMat;
}

void AudioManager::UpdateObjectTransform(AudioObject* obj)
{
    if (instance != nullptr)
    {
        for (auto& object : instance->objects)
        {
            if (object.second == obj)
            {
                iplInstancedMeshUpdateTransform(instance->steamStaticMeshes[object.first], instance->scene, ConvertGlmToIpl(obj->GetPosition(), obj->GetRotation()));
                iplSceneCommit(instance->scene);

                // FIX: Must commit the simulator when geometry moves to rebuild the BVH!
                iplSimulatorCommit(instance->simulator);
                return;
            }
        }
    }
}

void AudioManager::InitializeSteamAudioNode(const std::string& name)
{
    SteamAudioNode& node = steamAudioNodes[name];

    ma_uint32 inputChannels[1] = { 1 };
    ma_uint32 outputChannels[1] = { 2 };

    ma_node_config nodeConfig = ma_node_config_init();
    nodeConfig.vtable = &g_SteamAudioNodeVTable;
    nodeConfig.pInputChannels = inputChannels;
    nodeConfig.pOutputChannels = outputChannels;

    ma_result result = ma_node_init(ma_engine_get_node_graph(miniAudioEngine), &nodeConfig, NULL, &node.base);
    if (result != MA_SUCCESS) {
        Logger::Log("Failed to init miniaudio node for: " + name, Logger::Category::Error);
        return;
    }

    node.hrtf = hrtf;
    node.direction = { 0.0f, 0.0f, 1.0f };

    node.directParams.distanceAttenuation = 1.0f;
    node.directParams.occlusion = 0.0f;
    node.directParams.transmission[0] = 1.0f;
    node.directParams.transmission[1] = 1.0f;
    node.directParams.transmission[2] = 1.0f;

    // FIX 2: Add APPLYTRANSMISSION so the node actually uses your wall's material properties!
    node.directParams.flags = static_cast<IPLDirectEffectFlags>(
        IPL_DIRECTEFFECTFLAGS_APPLYDISTANCEATTENUATION |
        IPL_DIRECTEFFECTFLAGS_APPLYOCCLUSION |
        IPL_DIRECTEFFECTFLAGS_APPLYTRANSMISSION);

    IPLBinauralEffectSettings binSettings = { 0 };
    binSettings.hrtf = hrtf;
    iplBinauralEffectCreate(context, &iplSettings, &binSettings, &node.binauralEffect);

    IPLDirectEffectSettings dirSettings = { 0 };
    dirSettings.numChannels = 1;
    iplDirectEffectCreate(context, &iplSettings, &dirSettings, &node.directEffect);

    node.inputPlane = (float*)ma_malloc(iplSettings.frameSize * sizeof(float), NULL);
    node.midPlane = (float*)ma_malloc(iplSettings.frameSize * sizeof(float), NULL);
    node.outputPlanes[0] = (float*)ma_malloc(iplSettings.frameSize * sizeof(float), NULL);
    node.outputPlanes[1] = (float*)ma_malloc(iplSettings.frameSize * sizeof(float), NULL);

    memset(node.inputPlane, 0, iplSettings.frameSize * sizeof(float));
    memset(node.midPlane, 0, iplSettings.frameSize * sizeof(float));
    memset(node.outputPlanes[0], 0, iplSettings.frameSize * sizeof(float));
    memset(node.outputPlanes[1], 0, iplSettings.frameSize * sizeof(float));

    ma_node_attach_output_bus(&node.base, 0, ma_engine_get_endpoint(miniAudioEngine), 0);
}

void AudioManager::CleanupSource(const std::string& name) {
    if (steamSources.count(name)) {
        // FIX: Safely remove the source from the simulator before destroying it
        iplSourceRemove(steamSources[name], simulator);
        iplSimulatorCommit(simulator);

        iplSourceRelease(&steamSources[name]);
        steamSources.erase(name);
    }

    if (steamAudioNodes.count(name)) {
        SteamAudioNode& node = steamAudioNodes[name];

        iplBinauralEffectRelease(&node.binauralEffect);
        iplDirectEffectRelease(&node.directEffect);

        ma_free(node.inputPlane, NULL);
        ma_free(node.midPlane, NULL);
        ma_free(node.outputPlanes[0], NULL);
        ma_free(node.outputPlanes[1], NULL);

        ma_node_uninit(&node.base, NULL);
        steamAudioNodes.erase(name);
    }
}

void AudioManager::ClearAll()
{
    if (instance == nullptr) return;

    auto sourceIt = instance->sources.begin();
    while (sourceIt != instance->sources.end())
    {
        std::string name = sourceIt->first;
        Source* src = sourceIt->second;

        instance->CleanupSource(name);
        delete src;

        sourceIt = instance->sources.erase(sourceIt);
    }

    auto objIt = instance->objects.begin();
    while (objIt != instance->objects.end())
    {
        std::string name = objIt->first;
        AudioObject* obj = objIt->second;

        if (instance->steamStaticMeshes.count(name))
        {
            iplInstancedMeshRelease(&instance->steamStaticMeshes[name]);
            instance->steamStaticMeshes.erase(name);
        }

        delete obj;
        objIt = instance->objects.erase(objIt);
    }

    auto listIt = instance->listeners.begin();
    while (listIt != instance->listeners.end())
    {
        if (listIt->first != instance->activeListener)
        {
            delete listIt->second;
            listIt = instance->listeners.erase(listIt);
        }
        else
        {
            ++listIt;
        }
    }

    if (instance->scene)
    {
        iplSceneCommit(instance->scene);
    }

    Logger::Log("AudioManager: All audio assets cleared for level transition.", Logger::Category::Info);
}

ma_node* AudioManager::GetSteamAudioNodeBase(const std::string& name)
{
    if (instance != nullptr)
    {
        auto it = instance->steamAudioNodes.find(name);
        if (it != instance->steamAudioNodes.end())
        {
            return &it->second.base;
        }
    }
    return nullptr;
}
