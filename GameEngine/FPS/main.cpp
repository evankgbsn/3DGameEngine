#include "GameEngine/Engine.h"
#include "GameEngine/Scene/SceneManager.h"
#include "Scenes/Test.h"
#include "Scenes/FPSNetworkManager.h"
#include "Scenes/JoinScene.h"

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

	Scene* test = new Test();

	Scene* testScene = new Test();
	SceneManager::RegisterScene(testScene, "Test");

	Scene* joinScene = new JoinScene();
	SceneManager::RegisterScene(joinScene, "JoinScene");

	Scene* fpsNetworkManager = new FPSNetworkManager();
	SceneManager::RegisterScene(fpsNetworkManager, "FPSNetworkManager");

	SceneManager::LoadScene("JoinScene");
	SceneManager::InitializeScene("JoinScene");

	Engine::Run();

	Engine::Terminate();

	return 0;
}