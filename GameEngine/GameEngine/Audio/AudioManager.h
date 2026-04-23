#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include "../Utils/SingletonHelpers.h"
#include "AudioObject.h"

#include <miniaudio.h>
#include <phonon.h>
#include <glm/glm.hpp>

#include <unordered_map>

class Model;
class Listener;
class Source;

class AudioManager
{
public:

	static void Initialize();

	static void Terminate();

	static void EditorUpdate();

	static void GameUpdate();

	static ma_engine* GetMiniAudioEngine();

	static AudioObject* CreateAudioObject(const std::string& name, Model* const model, const glm::vec3& position, const glm::mat4& rotation, const AudioObject::Material& material);

	static AudioObject* GetAudioObject(const std::string& name);

	static Listener* CreateListener(const std::string& name, const glm::vec3& position, const glm::mat4& rotation);

	static Listener* GetListener(const std::string& name);

	static Listener* CreateSource(const std::string& name, const glm::vec3& position, const glm::mat4& rotation);

	static Listener* GetSource(const std::string& name);

	static void Delete(Listener* const listener);

	static void Delete(AudioObject* const object);

	static void Delete(const std::string& name);

	static void InitializeSteamAudioScene();

	static void SetActiveListener(const std::string& name);

private:

	friend class SingletonHelpers;

	friend class AudioObject;

	typedef struct {
		ma_node_base base;
		IPLHRTF hrtf;
		IPLBinauralEffect effect;

		// Direction vector updated by the main thread/game loop
		IPLVector3 direction;

		// Planar buffers for Steam Audio
		float* inputPlane;          // Mono input
		float* outputPlanes[2];     // Stereo (L/R) output
	} SteamAudioNode;

	AudioManager();

	~AudioManager();

	AudioManager(const AudioManager&) = delete;

	AudioManager& operator=(const AudioManager&) = delete;

	AudioManager(AudioManager&&) = delete;

	AudioManager& operator=(AudioManager&&) = delete;

	void InitializeMiniAudio();

	void InitializeSteamAudio();

	void SteamAudioNodeProcessPCMFrames(ma_node* pNode, const float** ppFramesIn, ma_uint32* pFrameCountIn, float** ppFramesOut, ma_uint32* ppFrameCountOut);

	static IPLMatrix4x4 ConvertGlmToIpl(const glm::vec3& position, const glm::mat4& rotation);

	static void UpdateObjectTransform(AudioObject* obj);

	static const IPLContext& GetSteamAudioContext();

	void UpdateListener();

	static AudioManager* instance;

	IPLAudioSettings iplSettings = { 48000, 1024 };

	IPLContext context;

	IPLHRTF hrtf;

	IPLSimulator simulator;

	IPLScene scene;

	IPLContextSettings contextSettings = { STEAMAUDIO_VERSION };

	IPLHRTFSettings hrtfSettings = { IPL_HRTFTYPE_DEFAULT, NULL, 0, 1.0f };

	IPLSimulationSettings simulatorSettings;

	ma_engine* miniAudioEngine;

	std::unordered_map<std::string, AudioObject*> objects;

	std::unordered_map<std::string, IPLInstancedMesh> steamStaticMeshes;

	std::unordered_map<std::string, Listener*> listeners;

	std::string activeListener;

	std::unordered_map<std::string, Source*> sources;

	std::unordered_map<std::string, IPLSource> steamSources;
};

#endif // AUDIOMANAGER_H