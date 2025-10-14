#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include <atomic>
#include <glm/glm.hpp>
#include <functional>
#include <unordered_map>

struct GLFWwindow;

class Window
{
public:

	unsigned int GetWidth() const;

	unsigned int GetHeight() const;

	glm::vec2 GetCursorPosition() const;

	void SetCursorPosition(const glm::vec2& newPos);

	bool GetCursorMoved(glm::vec2& outNewPosition) const;

	int GetKey(int keyCode, bool clearFrameKeyStates = false) const;

	int GetMouseButton(int mouseButton, bool clearFrameMouseButtonStates = false) const;

	void RegisterCallbackForMouseScroll(const std::string& name, std::function<void(double scrollx, double scrolly)>* const callback);

	void DeregisterCallbackForMouseScroll(const std::string& name);

	GLFWwindow* GetGLFWwindow() const;

	void DisableCursor() const;

	void EnableCursor() const;

	static std::unordered_map<GLFWwindow*, std::unordered_map<std::string, std::function<void(double, double)>*>> mouseScrollCallbacks;

	static std::unordered_map<GLFWwindow*, std::unordered_map<std::string, std::function<void(unsigned int, unsigned int)>*>> windowResizeCallbacks;

	void RegisterCallbackForWindowResize(const std::string& name, std::function<void(unsigned int width, unsigned int height)>* const callback);

	void DeregisterCallbackForWindowResize(const std::string& name);

	std::string GetTitle() const;

	void SetTitle(const std::string& newTitle);

private:

	friend class WindowManager;

	Window(unsigned int width, unsigned int height, const std::string& name);

	Window() = delete;

	~Window();

	Window(const Window&) = delete;
	
	Window& operator=(const Window&) = delete;

	Window(Window&&) = delete;

	Window& operator=(Window&&) = delete;

	bool ShouldClose();

	void Update();

	void InitializeGLEW();

	glm::vec2 lastFramePosition;

	GLFWwindow* glfwWindow;

	std::atomic<bool> shouldClose;
	
};

#endif WINDOW_H