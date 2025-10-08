#ifndef SURVIVALSERVERFREECAMERA_H
#define SURVIVALSERVERFREECAMERA_H

#include "GameEngine/GameObject/GameObject.h"

#include <functional>

class CameraComponent;

class SurvivalServerFreeCamera : public GameObject
{
public:

	SurvivalServerFreeCamera();

	~SurvivalServerFreeCamera();

private:

	SurvivalServerFreeCamera(const SurvivalServerFreeCamera&) = delete;

	SurvivalServerFreeCamera& operator=(const SurvivalServerFreeCamera&) = delete;

	SurvivalServerFreeCamera(SurvivalServerFreeCamera&&) = delete;

	SurvivalServerFreeCamera& operator=(SurvivalServerFreeCamera&&) = delete;

	void Initialize() override;

	void Terminate() override;

	void GameUpdate() override;

	void EditorUpdate() override;

	void Load() override;

	void Unload() override;

	void Start() override;

	void InputRegistration();

	void InputDeregistration();

	void CreateInputFunctions();
	
	CameraComponent* cam;

	std::function<void(int keyCode)>* wPress;

	std::function<void(int keyCode)>* aPress;

	std::function<void(int keyCode)>* sPress;

	std::function<void(int keyCode)>* dPress;

	std::function<void(int keyCode)>* qPress;

	std::function<void(int keyCode)>* ePress;

	std::function<void(int keyCode)>* zPress;

	std::function<void(int keyCode)>* cPress;

	std::function<void(int keyCode)>* ctrPress;

	std::function<void(int keyCode)>* spacePress;

	std::function<void(int keyCode)>* escPress;
};

#endif // SURVIVALSERVERFREECAMERA_H