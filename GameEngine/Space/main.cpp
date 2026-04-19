#include "GameEngine/Engine.h"

#include "GameEngine/Scene/SceneManager.h"
#include "GameEngine/Scene/Scene.h"
#include "Scenes/SpaceNetworkManager.h"
#include "Scenes/MainMenu.h"
#include "Scenes/AsteroidField.h"


int main(int argc, const char** argv)
{
	Engine::Initialize();

	Scene* asteroidField = new AsteroidField();
	SceneManager::RegisterScene(asteroidField, "AsteroidField");

	Scene* mainMenu = new MainMenu();
	SceneManager::RegisterScene(mainMenu, "MainMenu");

	Scene* spaceNetworkManager = new SpaceNetworkManager();
	SceneManager::RegisterScene(spaceNetworkManager, "SpaceNetworkManager");

	SceneManager::LoadScene("MainMenu");
	SceneManager::InitializeScene("MainMenu");

	if (!Engine::IsEditorBuild())
	{
		SceneManager::StartInitializedScenes();
	}

	Engine::Run();

	Engine::Terminate();

	return 0;
}