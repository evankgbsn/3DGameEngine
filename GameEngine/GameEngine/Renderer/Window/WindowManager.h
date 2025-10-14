#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include "Window.h"

#include <string>
#include <unordered_map>
#include <atomic>

class WindowManager
{
public:

	static Window* const GetWindow(const std::string& name);

	static void SetWindowTitle(const std::string& name, const std::string& newTitle);

	static std::string GetWindowTitle(const std::string& name);

private:

	friend class SingletonHelpers;

	friend class Renderer;

	static void Initialize();

	static void Terminate();

	static void CreateWindow(unsigned int width, unsigned int height, const std::string& name);

	static void Update();

	static bool ShouldTerminate();

	static WindowManager* instance;

	WindowManager();

	~WindowManager();

	WindowManager(const WindowManager&) = delete;

	WindowManager& operator=(const WindowManager&) = delete;

	WindowManager(WindowManager&&) = delete;

	WindowManager& operator=(WindowManager&&) = delete;

	void DeleteWindows();

	std::unordered_map<std::string, Window*> windows;

	std::atomic<bool> shouldTerminate;

	Window* mainWindow;

};

#endif // WINDOWMANAGER_H