#include "GameEngine/Engine.h"

#include "GameEngine/Scene/SceneManager.h"

#include "Scenes/HackerRunMain.h"

int main(int argc, const char** argv)
{
	Engine::Initialize();

	HackerRunMain mainScene;
	SceneManager::RegisterScene(&mainScene, "Main");
	SceneManager::LoadScene("Main");
	SceneManager::InitializeScene("Main");

	if (!Engine::IsEditorBuild())
	{
		SceneManager::StartInitializedScenes();
	}

	Engine::Run();
	
	Engine::Terminate();

	return 0;
}