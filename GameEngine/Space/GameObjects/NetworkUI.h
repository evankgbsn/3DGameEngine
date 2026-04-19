#ifndef NETWORKUI_H
#define NETWORKUI_H

#include "GameEngine/GameObject/GameObject.h"

#include <functional>

class TextField;
class Button;

class NetworkUI : public GameObject
{
public:

	NetworkUI();

	~NetworkUI();

private:

	NetworkUI(const NetworkUI&) = delete;

	NetworkUI& operator=(const NetworkUI&) = delete;

	NetworkUI(NetworkUI&&) = delete;

	NetworkUI& operator=(NetworkUI&&) = delete;

	void Initialize() override;

	void Terminate() override;

	void GameUpdate() override;

	void EditorUpdate() override;

	void Load() override;

	void Unload() override;

	void Start() override;

	TextField* text;

	Button* button;

	std::function<void()>* buttonFunc;
};

#endif // NETWORKUI_H