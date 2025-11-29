#include "GameEngine/Engine.h"
#include "GameEngine/Scene/SceneManager.h"
#include "Scenes/Test.h"

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

	SceneManager::RegisterScene(test, "Test");
	SceneManager::LoadScene("Test");
	SceneManager::InitializeScene("Test");

	Engine::Run();

	Engine::Terminate();

	return 0;
}