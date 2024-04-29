#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include <atomic>
#include <glm/glm.hpp>

struct GLFWwindow;

class Window
{
public:

	unsigned int GetWidth() const;

	unsigned int GetHeight() const;

	glm::vec2 GetCursorPosition() const;

	bool GetCursorMoved(glm::vec2& outNewPosition) const;

	int GetKey(int keyCode, bool clearFrameKeyStates = false) const;

	GLFWwindow* GetGLFWwindow() const;

	void DisableCursor() const;

	void EnableCursor() const;


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