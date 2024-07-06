#ifndef FREECAMERA_H
#define FREECAMERA_H

#include "GameEngine/GameObject/GameObject.h"

#include <functional>

class FreeCamera : public GameObject
{

public:

	FreeCamera();

	~FreeCamera();

private:
	
	FreeCamera(const FreeCamera&) = delete;

	FreeCamera& operator=(const FreeCamera&) = delete;

	FreeCamera(FreeCamera&&) = delete;

	FreeCamera& operator=(FreeCamera&&) = delete;

	void Initialize() override;

	void Terminate() override;

	void GameUpdate() override;

	void EditorUpdate() override;

	void Load() override;

	void Unload() override;

	void InputRegistration();

	void InputDeregistration();

	void CreateInputFunctions();

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

#endif // FREECAMERA_H