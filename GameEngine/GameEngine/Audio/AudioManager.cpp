#include "AudioManager.h"

#include "../Utils/Logger.h"
#include "AudioObject.h"
#include "Listener.h"

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

void AudioManager::GameUpdate()
{
    if (instance != nullptr)
    {
        instance->UpdateListener();
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

Listener* AudioManager::CreateSource(const std::string& name, const glm::vec3& position, const glm::mat4& rotation)
{
    return nullptr;
}

Listener* AudioManager::GetSource(const std::string& name)
{
    return nullptr;
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
                instance->objects.erase(instance->objects.find(listener.first));
                return;
            }
        }
    }
}

void AudioManager::Delete(AudioObject* const obj)
{
    if (instance != nullptr)
    {
        for (auto& object : instance->objects)
        {
            if (object.second == obj)
            {
                delete obj;
                instance->objects.erase(instance->objects.find(object.first));
                return;
            }
        }
    }
}

void AudioManager::Delete(const std::string& name)
{
    if (instance != nullptr)
    {
        bool audioObjectDeleted = false;
        if (instance->objects.find(name) != instance->objects.end())
        {
            delete instance->objects[name];
            audioObjectDeleted = true;
        }

        if (audioObjectDeleted)
        {
            instance->objects.erase(instance->objects.find(name));
        }

        bool listenerDeleted = false;
        if (instance->listeners.find(name) != instance->listeners.end())
        {
            if (name == instance->activeListener)
            {
                Logger::Log("Atempting to delete active listener. You cannot delete the active listener. There always needs to be an active listener.", Logger::Category::Error);
                return;
            }

            delete instance->objects[name];
            listenerDeleted = true;
        }

        if (listenerDeleted)
        {
            instance->objects.erase(instance->objects.find(name));
        }
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

void AudioManager::InitializeSteamAudio()
{
    iplContextCreate(&contextSettings, &context);

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
            instSettings.subScene = *object.second->steamSubscene;
            instSettings.transform = ConvertGlmToIpl(object.second->GetPosition(), object.second->GetRotation());

            iplInstancedMeshCreate(instance->scene, &instSettings, &instance->steamStaticMeshes[object.first]);
        }

        // 3. Commit the Scene (Builds the BVH)
        iplSceneCommit(instance->scene);
        iplSimulatorSetScene(instance->simulator, instance->scene);
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

    // 4. Apply the Steam Audio spatialization
    // This transforms the mono inputPlane into the stereo outputPlanes
    iplBinauralEffectApply(pSteamNode->effect, &params, &inBuffer, &outBuffer);

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
