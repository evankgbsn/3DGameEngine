#ifndef ENGINE_H
#define ENGINE_H

#include <functional>

class WindowManager;

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

	std::function<void(int keyCode)>* wPress;
	std::function<void(int keyCode)>* aPress;
	std::function<void(int keyCode)>* sPress;
	std::function<void(int keyCode)>* dPress;
	std::function<void(int keyCode)>* qPress;
	std::function<void(int keyCode)>* ePress;
	std::function<void(int keyCode)>* zPress;
	std::function<void(int keyCode)>* cPress;
	std::function<void(int keyCode)>* ctrPress;
	std::function<void(int keyCode)>* spacePress;
	std::function<void(int keyCode)>* iPress;
	std::function<void(int keyCode)>* jPress;
};

#endif // ENGINE_H