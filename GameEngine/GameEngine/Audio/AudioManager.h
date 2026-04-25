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
class Sound;

class AudioManager
{
public:

	static void Initialize();
	static void Terminate();
	static void EditorUpdate();
	static void GameUpdate();
	static ma_engine* GetMiniAudioEngine();
	static AudioObject* CreateAudioObject(const std::string& name, const Model* const model, const glm::vec3& position, const glm::mat4& rotation, const AudioObject::Material& material);
	static AudioObject* GetAudioObject(const std::string& name);
	static Listener* CreateListener(const std::string& name, const glm::vec3& position, const glm::mat4& rotation);
	static Listener* GetListener(const std::string& name);
	static Source* CreateSource(const std::string& name, const glm::vec3& position, const glm::mat4& rotation);
	static Source* GetSource(const std::string& name);
	static Sound* LoadSound(const std::string& name, const std::string& path);
	static Sound* GetSound(const std::string& name);
	static bool SoundLoaded(const std::string& name);
	static void UnloadSound(const std::string& name);
	static void Delete(Listener* const listener);
	static void Delete(AudioObject* const object);
	static void Delete(Source* const source);
	static void Delete(const std::string& name);
	static void InitializeSteamAudioScene();
	static void SetActiveListener(const std::string& name);
	static void SteamAudioNodeProcessPCMFrames(ma_node* pNode, const float** ppFramesIn, ma_uint32* pFrameCountIn, float** ppFramesOut, ma_uint32* ppFrameCountOut);
	static void ClearAll();
	static ma_node* GetSteamAudioNodeBase(const std::string& name);
	static IPLEmbreeDevice GetEmbreeDevice();

private:

	friend class SingletonHelpers;
	friend class AudioObject;

	typedef struct {
		ma_node_base base;
		IPLHRTF hrtf;
		IPLBinauralEffect binauralEffect;
		IPLDirectEffect directEffect;

		IPLVector3 direction;
		IPLDirectEffectParams directParams;

		float* inputPlane;
		float* midPlane; // NEW: Buffer to sit between Direct and Binaural effects
		float* outputPlanes[2];
	} SteamAudioNode;

	AudioManager();
	~AudioManager();
	AudioManager(const AudioManager&) = delete;
	AudioManager& operator=(const AudioManager&) = delete;
	AudioManager(AudioManager&&) = delete;
	AudioManager& operator=(AudioManager&&) = delete;

	void InitializeMiniAudio();
	void InitializeSteamAudio();
	static IPLMatrix4x4 ConvertGlmToIpl(const glm::vec3& position, const glm::mat4& rotation);
	static void UpdateObjectTransform(AudioObject* obj);
	static const IPLContext& GetSteamAudioContext();
	void UpdateListener();
	void UpdateSources();
	void ProccessUpdatedSounds();
	void InitializeSteamAudioNode(const std::string& name);
	void CleanupSource(const std::string& name);

	static AudioManager* instance;

	IPLEmbreeDevice embreeDevice;
	IPLAudioSettings iplSettings;
	IPLContext context;
	IPLHRTF hrtf;
	IPLSimulator simulator;
	IPLScene scene;
	IPLContextSettings contextSettings;
	IPLHRTFSettings hrtfSettings;
	IPLSimulationSettings simulatorSettings;
	ma_engine* miniAudioEngine;

	std::unordered_map<std::string, AudioObject*> objects;
	std::unordered_map<std::string, IPLInstancedMesh> steamStaticMeshes;
	std::unordered_map<std::string, Listener*> listeners;
	std::string activeListener;
	std::unordered_map<std::string, Source*> sources;
	std::unordered_map<std::string, IPLSource> steamSources;
	std::unordered_map<std::string, SteamAudioNode> steamAudioNodes;
	std::unordered_map<std::string, Sound*> sounds;
};

#endif // AUDIOMANAGER_H