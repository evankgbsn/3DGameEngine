#ifndef MAINMENU_H
#define MAINMENU_H

#include "GameEngine/Scene/Scene.h"

class NetworkUI;

class MainMenu : public Scene
{
public:

	MainMenu();

	~MainMenu();

private:

	MainMenu(const MainMenu&) = delete;

	MainMenu& operator=(const MainMenu&) = delete;

	MainMenu(MainMenu&&) = delete;

	MainMenu& operator=(MainMenu&&) = delete;

	void Initialize() override;

	void Terminate() override;

	void Load() override;

	void Unload() override;

	void Start() override;

	void End() override;

	NetworkUI* networkUI;

};

#endif // MAINMENU_H