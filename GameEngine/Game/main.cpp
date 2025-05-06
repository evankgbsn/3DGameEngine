#include "GameEngine/Engine.h"
#include "Scenes/MainScene.h"
#include "Scenes/Subscene.h"
#include "Scenes/PhysXTest.h"
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
	Scene* subscene = new Subscene();
	Scene* physXTest = new PhysXTest();
	SceneManager::RegisterScene(mainScene, "Main");
	SceneManager::RegisterScene(subscene, "Sub");
	SceneManager::RegisterScene(physXTest, "PhysXTest");
	SceneManager::LoadScene("PhysXTest");

	Engine::Run();

	delete physXTest;
	delete subscene;
	delete mainScene;

	Engine::Terminate();

	return 0;
}