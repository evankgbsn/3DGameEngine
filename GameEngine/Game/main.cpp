#include "GameEngine/Engine.h"
#include "Scenes/MainScene.h"
#include "GameEngine/Scene/SceneManager.h"

class HeapProfile
{
public:
	~HeapProfile()
	{
	
	}
};

static HeapProfile profile;

int main(int argc, const char** argv)
{
	Engine::Initialize();

	Scene* mainScene = new MainScene();
	SceneManager::RegisterScene(mainScene, "Main");
	SceneManager::LoadScene("Main");

	Engine::Run();

	delete mainScene;

	Engine::Terminate();

	return 0;
}