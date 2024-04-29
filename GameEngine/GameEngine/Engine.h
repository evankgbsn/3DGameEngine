#ifndef ENGINE_H
#define ENGINE_H

#include <functional>

class WindowManager;
class AnimatedCollider;
class GOTexturedLit;
class GOTexturedAnimatedLit;

class Engine
{
public:

	static void Initialize();

	static void Run();

	static void Terminate();

	static WindowManager* GetWindowManager();

private:

	friend class SingletonHelpers;

	Engine();

	~Engine();

	static Engine* instance;
};

#endif // ENGINE_H