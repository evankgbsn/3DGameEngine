#include "../GameEngine/Engine.h"

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
	Engine::Run();
	Engine::Terminate();

	return 0;
}