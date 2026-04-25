#include "AudioManager.h"

#include "../Utils/Logger.h"
#include "AudioObject.h"
#include "Listener.h"
#include "Source.h"
#include "Sound.h"

// This is the "Blueprint" for your custom node.
// It must be static or global so the pointer remains valid.
static ma_node_vtable g_SteamAudioNodeVTable = {
    // 1. Point to your processing function
    // Note: We use a static wrapper to call your class member if needed
    AudioManager::SteamAudioNodeProcessPCMFrames,

    nullptr, // onGetInertia (Not needed for this)
    1,       // 1 Input Bus (Mono)
    1,       // 1 Output Bus (Stereo)
    MA_NODE_FLAG_CONTINUOUS_PROCESSING
};

AudioManager* AudioManager::instance = nullptr;

void AudioManager::Initialize()
{
	SingletonHelpers::InitializeSingleton(&instance, "Initialized AudioManager");
}

void AudioManager::Terminate()
{
    instance->ClearAll();
	SingletonHelpers::TerminateSingleton(&instance, "Terminated AudioManager");
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
    sharedInputs.listener.origin = {listenerPos.x, listenerPos.y, listenerPos.z};
    sharedInputs.listener.ahead = {listenerForward.x, listenerForward.y, listenerForward.z};
    sharedInputs.listener.up = {listenerUp.x, listenerUp.y, listenerUp.z};
    sharedInputs.listener.right = {listenerRight.x, listenerRight.y, listenerRight.z};

    iplSimulatorSetSharedInputs(simulator, IPL_SIMULATIONFLAGS_DIRECT, &sharedInputs);
}

void AudioManager::UpdateSources()
{
    for (const auto& source : sources)
    {
        IPLSimulationInputs inputs = { };
        inputs.flags = IPL_SIMULATIONFLAGS_DIRECT;
        inputs.directFlags = static_cast<IPLDirectSimulationFlags>(
            IPL_DIRECTSIMULATIONFLAGS_OCCLUSION |
            IPL_DIRECTSIMULATIONFLAGS_TRANSMISSION
            );
        inputs.source.origin = { source.second->GetPosition().x, source.second->GetPosition().y, source.second->GetPosition().z };
        inputs.source.ahead = { source.second->GetForward().x, source.second->GetForward().y, source.second->GetForward().z }; // Optional for directional sources

        iplSourceSetInputs(steamSources[source.first], IPL_SIMULATIONFLAGS_DIRECT, &inputs);
    }
}

void AudioManager::ProccessUpdatedSounds()
{
    glm::vec3 L = instance->listeners[instance->activeListener]->GetPosition();

    // Loop through every active source to poll results
    for (auto& sourcePair : instance->sources)
    {
        const std::string& name = sourcePair.first;
        Source* src = sourcePair.second;

        IPLSimulationOutputs outputs = { };
        iplSourceGetOutputs(instance->steamSources[name], IPL_SIMULATIONFLAGS_DIRECT, &outputs);

        // 2. Calculate Relative Direction Vector
                // Vector from Listener to Source
        glm::vec3 S = src->GetPosition();
        glm::vec3 relativeDir = glm::normalize(S - L);

        // Access the node from your map and push the results
        // Note: Ensure you check if the node exists in the map first
        if (steamAudioNodes.count(name)) {
            steamAudioNodes[name].directParams = outputs.direct;
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

AudioObject* AudioManager::CreateAudioObject(const std::string& name, Model* const model, const glm::vec3& position, const glm::mat4& rotation, const AudioObject::Material& material)
{
    AudioObject* ret = nullptr;

    if (instance != nullptr)
    {
        if (instance->objects.find(name) == instance->objects.end())
        {
            ret = instance->objects[name] = new AudioObject(model, position, rotation, material);
        }
        else
        {
            Logger::Log("Audio object with the name " + name + " already exsists.", Logger::Category::Warning);
            ret = instance->objects[name];
        }
    }
    else
    {
        Logger::Log("Calling AudioManager::CreateAudioObject before AudioManager::Initialize()", Logger::Category::Warning);
    }

    return ret;
}

AudioObject* AudioManager::GetAudioObject(const std::string& name)
{
    AudioObject* ret = nullptr;

    if (instance != nullptr)
    {
        if (instance->objects.find(name) != instance->objects.end())
        {
            ret = instance->objects[name];
        }
        else
        {
            Logger::Log("Could not find AudioObject with name: " + name + " AudioManager::GetAudioObject()", Logger::Category::Warning);
        }
    }
    else
    {
        Logger::Log("Calling AudioManager::GetAudioObject before AudioManager::Initialize()", Logger::Category::Warning);
    }

    return ret;
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
            Logger::Log("Listener with the name " + name + " already exsists.", Logger::Category::Warning);
            ret = instance->listeners[name];
        }
    }
    else
    {
        Logger::Log("Calling AudioManager::CreateListener before AudioManager::Initialize()", Logger::Category::Warning);
    }

    return ret;
}

Listener* AudioManager::GetListener(const std::string& name)
{
    Listener* ret = nullptr;

    if (instance != nullptr)
    {
        if (instance->listeners.find(name) != instance->listeners.end())
        {
            ret = instance->listeners[name];
        }
        else
        {
            Logger::Log("Could not find Listener with name: " + name + " AudioManager::GetListener()", Logger::Category::Warning);
        }
    }
    else
    {
        Logger::Log("Calling AudioManager::GetListener before AudioManager::Initialize()", Logger::Category::Warning);
    }

    return ret;
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
            sourceSettings.flags = IPL_SIMULATIONFLAGS_DIRECT; // Enables Occlusion/Transmission
            iplSourceCreate(instance->simulator, &sourceSettings, &instance->steamSources[name]);
            instance->steamAudioNodes[name];
            instance->InitializeSteamAudioNode(name);
        }
        else
        {
            Logger::Log("Source with the name " + name + " already exsists.", Logger::Category::Warning);
            ret = instance->sources[name];
        }
    }
    else
    {
        Logger::Log("Calling AudioManager::CreateSource before AudioManager::Initialize()", Logger::Category::Warning);
    }

    return ret;
}

Source* AudioManager::GetSource(const std::string& name)
{
    Source* ret = nullptr;

    if (instance != nullptr)
    {
        if (instance->sources.find(name) != instance->sources.end())
        {
            ret = instance->sources[name];
        }
        else
        {
            Logger::Log("Could not find Source with name: " + name + " AudioManager::GetSource()", Logger::Category::Warning);
        }
    }
    else
    {
        Logger::Log("Calling AudioManager::GetSource before AudioManager::Initialize()", Logger::Category::Warning);
    }

    return ret;
}

Sound* AudioManager::LoadSound(const std::string& name, const std::string& path)
{
    Sound* ret = nullptr;

    if (instance != nullptr)
    {
        if (instance->sounds.find(name) == instance->sounds.end())
        {
            instance->sounds[name] = ret = new Sound(name, path);
        }
        else
        {
            ret = instance->sounds[name];
            Logger::Log("Sound: " + name + " has already been loaded.", Logger::Category::Warning);
        }
    }
    else
    {
        Logger::Log("Calling AudioManager::LoadSound() before AudioManager::Initialize()", Logger::Category::Error);
    }

    return ret;
}

Sound* AudioManager::GetSound(const std::string& name)
{
    Sound* ret = nullptr;

    if (instance != nullptr)
    {
        if (instance->sounds.find(name) != instance->sounds.end())
        {
            ret = instance->sounds[name];
        }
        else
        {
            Logger::Log("Sound: " + name + " has not been loaded. Cannot get sound", Logger::Category::Warning);
        }
    }
    else
    {
        Logger::Log("Calling AudioManager::GetSound() before AudioManager::Initialize()", Logger::Category::Error);
    }

    return ret;
}

bool AudioManager::SoundLoaded(const std::string& name)
{
    if (instance != nullptr)
    {
        return instance->sounds.find(name) != instance->sounds.end();
    }
    else
    {
        Logger::Log("Calling AudioManager::SoundLoaded() before AudioManager::Initialize()", Logger::Category::Error);
    }

    return false;
}

void AudioManager::UnloadSound(const std::string& name)
{
    if (instance != nullptr)
    {
        if (instance->sounds.find(name) != instance->sounds.end())
        {
            delete instance->sounds[name];
            instance->sounds.erase(instance->sounds.find(name));
            Logger::Log("Unloaded Sound: " + name, Logger::Category::Success);
        }
        else
        {
            Logger::Log("Sound: " + name + " has not been loaded. You cannot unload a sound that has not been loaded.", Logger::Category::Warning);
        }
    }
    else
    {
        Logger::Log("Calling AudioManager::UnloadSound() before AudioManager::Initialize()", Logger::Category::Error);
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
                    Logger::Log("Atempting to delete active listener. You cannot delete the active listener. There always needs to be an active listener.", Logger::Category::Error);
                    return;
                }
                delete l;
                instance->listeners.erase(instance->listeners.find(listener.first));
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

                // 1. Release the Instance Handle from the World Scene
                if (instance->steamStaticMeshes.count(name)) {
                    iplInstancedMeshRelease(&instance->steamStaticMeshes[name]);
                    instance->steamStaticMeshes.erase(name);
                }

                // 2. Delete the object (triggers AudioObject destructor)
                delete obj; // cleans up Sub-Scene & Static Mesh

                // 3. Commit the Main Scene change
                iplSceneCommit(instance->scene);

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
                delete source;
                instance->CleanupSource(src.first);
                instance->sources.erase(instance->sources.find(src.first));
                return;
            }
        }
    }
}

void AudioManager::Delete(const std::string& name)
{
    if (instance == nullptr) return;

    bool sceneChanged = false;

    // 1. Handle AudioObject Deletion
    if (instance->objects.count(name))
    {
        // First, remove the Instance from the Main Scene
        if (instance->steamStaticMeshes.count(name))
        {
            iplInstancedMeshRelease(&instance->steamStaticMeshes[name]);
            instance->steamStaticMeshes.erase(name);
            sceneChanged = true;
        }

        // Delete calls the AudioObject destructor (Releases Sub-scene & Mesh)
        delete instance->objects[name];
        instance->objects.erase(name);
    }

    // 2. Handle Source Deletion
    if (instance->sources.count(name))
    {
        // CleanupSource handles iplSourceRelease and Node memory
        instance->CleanupSource(name);

        delete instance->sources[name];
        instance->sources.erase(name);
    }

    // 3. Handle Listener Deletion
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

    // 4. Commit changes to the World Scene if any geometry was removed
    if (sceneChanged)
    {
        iplSceneCommit(instance->scene);
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
    // 1. Clean up all active sources/objects/listeners first
    for (const auto& source : sources)
    {
        CleanupSource(source.first);
    }

    // 2. Release Steam Audio Global handles
    iplSimulatorRelease(&simulator);
    iplSceneRelease(&scene);
    iplHRTFRelease(&hrtf);
    iplContextRelease(&context);

    // 3. Miniaudio cleanup
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

	ma_result result = ma_engine_init(nullptr, miniAudioEngine);

	if (result != MA_SUCCESS)
	{
		Logger::Log(std::string("Failed to initialize MiniAudioEngine"), Logger::Category::Error);
	}

    ma_engine_set_volume(AudioManager::GetMiniAudioEngine(), 1.0f);
}

void AudioManager::InitializeSteamAudio()
{
    iplContextCreate(&contextSettings, &context);

    // 1. Setup Context Settings
    contextSettings.version = STEAMAUDIO_VERSION;

    // 2. Setup Global Audio Settings
    iplSettings.samplingRate = 48000;
    iplSettings.frameSize = 1024;

    iplContextCreate(&contextSettings, &context);

    // 3. Setup HRTF Settings
    hrtfSettings.type = IPL_HRTFTYPE_DEFAULT;
    hrtfSettings.sofaFileName = nullptr;
    // Remove numSpeakers and ambisonicsOrder
    hrtfSettings.volume = 1.0f;

    iplHRTFCreate(context, &iplSettings, &hrtfSettings, &hrtf);

    // 3. Simulator (The brain that handles raycasting)
    simulatorSettings = { };
    simulatorSettings.maxNumSources = 64; // Max spatial sounds
    simulatorSettings.numThreads = 2;     // Background threads for raycasting
    iplSimulatorCreate(context, &simulatorSettings, &simulator);
}

void AudioManager::InitializeSteamAudioScene()
{
    if (instance != nullptr)
    {
        // 1. Create a Scene
        IPLSceneSettings sceneSettings = { IPL_SCENETYPE_DEFAULT };
        iplSceneCreate(AudioManager::GetSteamAudioContext(), &sceneSettings, &instance->scene);

        for (auto& object : instance->objects)
        {
            IPLInstancedMeshSettings instSettings{};
            instSettings.subScene = object.second->steamSubscene;
            instSettings.transform = ConvertGlmToIpl(object.second->GetPosition(), object.second->GetRotation());

            iplInstancedMeshCreate(instance->scene, &instSettings, &instance->steamStaticMeshes[object.first]);
        }

        // 3. Commit the Scene (Builds the BVH)
        iplSceneCommit(instance->scene);
        iplSimulatorSetScene(instance->simulator, instance->scene);
    }
}

void AudioManager::SetActiveListener(const std::string& name)
{
    if (instance != nullptr)
    {
        if (instance->listeners.find(name) != instance->listeners.end())
        {
            instance->activeListener = name;
        }
        else
        {
            Logger::Log("Cannot set active listener, Listener: " + name + " does not exsist.", Logger::Category::Error);
        }
    }
    else
    {
        Logger::Log("Calling AudioManager::SetActiveListener() before AudioManager::Initialize()", Logger::Category::Error);
    }
}

void AudioManager::SteamAudioNodeProcessPCMFrames(ma_node* pNode, const float** ppFramesIn, ma_uint32* pFrameCountIn, float** ppFramesOut, ma_uint32* pFrameCountOut)
{
    SteamAudioNode* pSteamNode = (SteamAudioNode*)pNode;

    // 1. Prepare Steam Audio Effect Parameters
    // In a real engine, 'direction' should be calculated 
    // from the listener's relative coordinate space.
    IPLBinauralEffectParams params = { 0 };
    params.hrtf = pSteamNode->hrtf;
    params.direction = pSteamNode->direction; // IPLVector3
    params.interpolation = IPL_HRTFINTERPOLATION_BILINEAR;
    params.spatialBlend = 1.0f;

    // 2. Wrap our pre-allocated plane pointers into IPLAudioBuffer structs
    // Note: .data is a float**, so we pass the address of our pointers.
    IPLAudioBuffer inBuffer;
    inBuffer.numChannels = 1;
    inBuffer.data = &pSteamNode->inputPlane;

    IPLAudioBuffer outBuffer;
    outBuffer.numChannels = 2;
    outBuffer.data = pSteamNode->outputPlanes; // outputPlanes is already float* [2]

    // 3. De-interleave miniaudio input into Steam Audio's input plane
    // Even if the input is mono, we use this to safely copy data.
    ma_deinterleave_pcm_frames(
        ma_format_f32,
        1,                       // Mono source
        *pFrameCountIn,
        (const void*)ppFramesIn[0],
        (void**)&pSteamNode->inputPlane
    );

    // 1. Apply Occlusion/Transmission/Distance Attenuation first
    // This uses the directParams you polled in the GameUpdate
    iplDirectEffectApply(pSteamNode->directEffect, &pSteamNode->directParams, &inBuffer, &inBuffer);

    // 2. Then apply the Binaural HRTF to the already-muffled sound
    IPLBinauralEffectParams binParams = { 0 };
    binParams.direction = pSteamNode->direction; // Unit vector
    binParams.hrtf = pSteamNode->hrtf;

    // 4. Apply the Steam Audio spatialization
    // This transforms the mono inputPlane into the stereo outputPlanes
    iplBinauralEffectApply(pSteamNode->binauralEffect, &params, &inBuffer, &outBuffer);

    // 5. Interleave Steam Audio's stereo output back into miniaudio's output buffer
    ma_interleave_pcm_frames(
        ma_format_f32,
        2,                       // Stereo destination
        *pFrameCountOut,
        (const void**)pSteamNode->outputPlanes,
        (void*)ppFramesOut[0]
    );
}

const IPLContext& AudioManager::GetSteamAudioContext()
{
    static IPLContext defaultReturn;

    if (instance != nullptr)
    {
        return instance->context;
    }
    else
    {
        Logger::Log("Calling AudioManager::GetSteamAudioContext() before AudioManager::Initialize()", Logger::Category::Error);
    }

    return defaultReturn;
}

IPLMatrix4x4 AudioManager::ConvertGlmToIpl(const glm::vec3& position, const glm::mat4& rotation) {
    // 1. Start with the rotation matrix
    glm::mat4 model = rotation;

    // 2. Inject the position into the translation column
    model[3][0] = position.x;
    model[3][1] = position.y;
    model[3][2] = position.z;
    model[3][3] = 1.0f;

    // 3. Construct the IPLMatrix4x4
    // Steam Audio matrices are arrays of 4 arrays of 4 floats [row][column]
    IPLMatrix4x4 iplMat;

    // GLM stores data in Column-Major, but we access elements via [col][row].
    // Steam Audio expects the matrix to be laid out such that:
    // iplMat.elements[row][col]
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            // We flip the indices because GLM is [col][row] and IPL is [row][col]
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
                return;
            }
        }

    }
}

void AudioManager::InitializeSteamAudioNode(const std::string& name)
{
    SteamAudioNode& node = steamAudioNodes[name];

    ma_uint32 inputChannels[1] = { 1 };  // Mono
    ma_uint32 outputChannels[1] = { 2 }; // Stereo

    ma_node_config nodeConfig = ma_node_config_init();
    nodeConfig.vtable = &g_SteamAudioNodeVTable;
    nodeConfig.pInputChannels = inputChannels;   // Pointing to the array
    nodeConfig.pOutputChannels = outputChannels; // Pointing to the array

    ma_result result = ma_node_init(ma_engine_get_node_graph(miniAudioEngine), &nodeConfig, NULL, &node.base);
    if (result != MA_SUCCESS) {
        Logger::Log("Failed to init miniaudio node for: " + name, Logger::Category::Error);
        return;
    }

    // 2. Assign Global Handles
    node.hrtf = hrtf;

    // 3. Create Steam Audio Effects
    IPLBinauralEffectSettings binSettings{ hrtf };
    iplBinauralEffectCreate(context, &iplSettings, &binSettings, &node.binauralEffect);

    IPLDirectEffectSettings dirSettings{};
    dirSettings.numChannels = 1; // Direct effect processes the mono source
    iplDirectEffectCreate(context, &iplSettings, &dirSettings, &node.directEffect);

    // 4. Allocate Planar Memory (Aligning for SIMD is recommended)
    // iplSettings.frameSize (usually 1024) is the number of floats per channel
    node.inputPlane = (float*)ma_malloc(iplSettings.frameSize * sizeof(float), NULL);
    node.outputPlanes[0] = (float*)ma_malloc(iplSettings.frameSize * sizeof(float), NULL);
    node.outputPlanes[1] = (float*)ma_malloc(iplSettings.frameSize * sizeof(float), NULL);

    // Initialize planes to silence
    memset(node.inputPlane, 0, iplSettings.frameSize * sizeof(float));
    memset(node.outputPlanes[0], 0, iplSettings.frameSize * sizeof(float));
    memset(node.outputPlanes[1], 0, iplSettings.frameSize * sizeof(float));
}

void AudioManager::CleanupSource(const std::string& name) {
    // 1. Release Steam Audio Source handle
    if (steamSources.count(name)) {
        iplSourceRelease(&steamSources[name]);
        steamSources.erase(name);
    }

    // 2. Cleanup the Custom Node
    if (steamAudioNodes.count(name)) {
        SteamAudioNode& node = steamAudioNodes[name];

        // Release Effect handles
        iplBinauralEffectRelease(&node.binauralEffect);
        iplDirectEffectRelease(&node.directEffect);

        // Free the manual planar memory
        ma_free(node.inputPlane, NULL);
        ma_free(node.outputPlanes[0], NULL);
        ma_free(node.outputPlanes[1], NULL);

        // Uninitialize the miniaudio node base
        ma_node_uninit(&node.base, NULL);

        steamAudioNodes.erase(name);
    }
}

void AudioManager::ClearAll()
{
    if (instance == nullptr) return;

    // 1. Clear Sources and their associated SteamAudioNodes
    // We use a while loop because CleanupSource removes items from the map
    auto sourceIt = instance->sources.begin();
    while (sourceIt != instance->sources.end())
    {
        std::string name = sourceIt->first;
        Source* src = sourceIt->second;

        instance->CleanupSource(name);
        delete src;

        sourceIt = instance->sources.erase(sourceIt);
    }

    // 2. Clear AudioObjects and their World Instances
    auto objIt = instance->objects.begin();
    while (objIt != instance->objects.end())
    {
        std::string name = objIt->first;
        AudioObject* obj = objIt->second;

        // Release the Instance from the Main Scene
        if (instance->steamStaticMeshes.count(name))
        {
            iplInstancedMeshRelease(&instance->steamStaticMeshes[name]);
            instance->steamStaticMeshes.erase(name);
        }

        // Delete triggers AudioObject destructor (Releases Sub-scene/Mesh)
        delete obj;

        objIt = instance->objects.erase(objIt);
    }

    // 3. Clear Listeners (except the active one if you want to keep a camera)
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

    // 4. Final Scene Update
    // Re-committing an empty scene tells the simulator the world is now empty
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
        // Check if the node actually exists for this source
        auto it = instance->steamAudioNodes.find(name);
        if (it != instance->steamAudioNodes.end())
        {
            // Return the address of the miniaudio-compatible base struct
            return &it->second.base;
        }
        else
        {
            Logger::Log("Could not find SteamAudioNode for: " + name, Logger::Category::Warning);
        }
    }
    return nullptr;
}
