#ifndef ENGINE_H
#define ENGINE_H

#include <functional>

class WindowManager;
class AnimatedCollider;
class GOTexturedLit;
class GOTexturedAnimatedLit;
class Window;

class Engine
{
public:

	static void Initialize();

	static void Run();

	static void Terminate();

	static WindowManager* GetWindowManager();

	static Window* GetWindow();

private:

	friend class SingletonHelpers;

	Engine();

	~Engine();

	void EditorPlayToggleInputSetup();

	static Engine* instance;

	bool editorState;
};

#endif // ENGINE_H