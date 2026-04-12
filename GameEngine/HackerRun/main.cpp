#include "GameEngine/Engine.h"

int main(int argc, const char** argv)
{
	Engine::Initialize();

	Engine::Run();

	Engine::Terminate();

	return 0;
}