#include "GameEngine/Engine.h"
//#include "Scenes/MainScene.h"
//#include "Scenes/Subscene.h"
//#include "Scenes/PhysXTest.h"
//#include "Scenes/FPSTest.h"
#include "Scenes/SurvivalScene.h"
#include "GameEngine/Scene/SceneManager.h"
#include "Scenes/SurvivalLogin.h"
#include "Scenes/SurvivalNetworkManager.h"

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

	//Scene* mainScene = new MainScene();
	//Scene* subscene = new Subscene();
	//Scene* physXTest = new PhysXTest();
	//Scene* fpsTest = new FPSTest();
	//SceneManager::RegisterScene(mainScene, "Main");
	//SceneManager::RegisterScene(subscene, "Sub");
	//SceneManager::RegisterScene(physXTest, "PhysXTest");
	//SceneManager::RegisterScene(fpsTest, "FPSTest");

	Scene* survivalScene = new SurvivalScene();
	SceneManager::RegisterScene(survivalScene, "SurvivalScene");
	
	Scene* survivalLoginScene = new SurvivalLogin();
	SceneManager::RegisterScene(survivalLoginScene, "SurvivalLoginScene");

	Scene* survivalNetworkManager = new SurvivalNetworkManager();
	SceneManager::RegisterScene(survivalNetworkManager, "SurvivalNetworkManager");

	SceneManager::LoadScene("SurvivalLoginScene");
	SceneManager::InitializeScene("SurvivalLoginScene");

	Engine::Run();

	//delete fpsTest;
	//delete physXTest;
	//delete subscene;
	//delete mainScene;

	delete survivalScene;
	Engine::Terminate();

	return 0;
}