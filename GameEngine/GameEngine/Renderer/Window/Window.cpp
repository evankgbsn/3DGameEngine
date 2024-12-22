#include "Window.h"

#include "../../Utils/Logger.h"
#include "../Shader/ShaderManager.h"
#include "../Model/ModelManager.h"
#include "../Model/Model.h"
#include "../GraphicsObjects/GraphicsObjectManager.h"
#include "../Texture/TextureManager.h"
#include "../../Input/InputManager.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

std::unordered_map<GLFWwindow*, std::unordered_map<std::string, std::function<void(double, double)>*>> Window::mouseScrollCallbacks = std::unordered_map<GLFWwindow*, std::unordered_map<std::string, std::function<void(double, double)>*>>();

void WindowScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	for (auto& function : Window::mouseScrollCallbacks[window])
	{
		(*function.second)(xoffset, yoffset);
	}
}

unsigned int Window::GetWidth() const
{
	int width, height;
	glfwGetWindowSize(glfwWindow, &width, &height);
	return width;
}

unsigned int Window::GetHeight() const
{
	int width, height;
	glfwGetWindowSize(glfwWindow, &width, &height);
	return height;
}

Window::Window(unsigned int width, unsigned int height, const std::string& name) :
	glfwWindow(nullptr),
	shouldClose(false)
{
	int monitorCount;
	GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
	glfwWindow = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), name.c_str(), nullptr, nullptr);

	if (glfwWindow == nullptr)
	{
		Logger::Log("Failed to create window " + name, Logger::Category::Error);
	}
	else
	{
		glfwMakeContextCurrent(glfwWindow);
		glfwSwapInterval(0);
		InitializeGLEW();
		glfwSetScrollCallback(glfwWindow, WindowScrollCallback);
	}
}

Window::~Window()
{
	glfwDestroyWindow(glfwWindow);
}

bool Window::ShouldClose()
{
	return shouldClose.load();
}

void Window::Update()
{
	shouldClose.store(glfwWindowShouldClose(glfwWindow));
	if (!shouldClose.load())
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GraphicsObjectManager::Update();

		glfwSwapBuffers(glfwWindow);
		glfwPollEvents();
	}
}

void Window::InitializeGLEW()
{
	static bool initOnce = false;

	if (!initOnce)
	{
		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			/* Problem: glewInit failed, something is seriously wrong. */
			Logger::Log(std::string("Error: ") + (const char*)glewGetErrorString(err));
		}
		Logger::Log(std::string("Status: Using GLEW ") + (const char*)glewGetString(GLEW_VERSION));

		initOnce = true;
	}
}

glm::vec2 Window::GetCursorPosition() const
{
	double cursorX, cursorY;
	glfwGetCursorPos(glfwWindow, &cursorX, &cursorY);

	return glm::vec2(static_cast<float>(cursorX), static_cast<float>(cursorY));
}

void Window::SetCursorPosition(const glm::vec2& newPos)
{
	glfwSetCursorPos(glfwWindow, newPos.x, newPos.y);
}

bool Window::GetCursorMoved(glm::vec2& outNewPosition) const
{
	glm::vec2 currentCursorPosition = GetCursorPosition();
	float difX = max(abs(currentCursorPosition.x), abs(lastFramePosition.x)) - min(abs(currentCursorPosition.x), abs(lastFramePosition.x));
	float difY = max(abs(currentCursorPosition.y), abs(lastFramePosition.y)) - min(abs(currentCursorPosition.y), abs(lastFramePosition.y));

	float errorRange = 1.0f;
	if (!(difX < errorRange && difY < errorRange))
	{
		outNewPosition = currentCursorPosition;
		return true;
	}
	return false;
}

int Window::GetKey(int keyCode, bool clearFrameKeyStates) const
{
	static std::unordered_set<int> pressedKeys = {};
	static std::unordered_set<int> releasedKeys = {};

	static std::unordered_map<int, int> keyStatesForThisFrame;

	if (clearFrameKeyStates)
	{
		keyStatesForThisFrame.clear();
		return -1;
	}

	if (keyStatesForThisFrame.find(keyCode) != keyStatesForThisFrame.end())
	{
		return keyStatesForThisFrame[keyCode];
	}
	else
	{
		int getKeyResult = glfwGetKey(glfwWindow, keyCode);

		if (getKeyResult == KEY_PRESS)
		{

			if (!pressedKeys.contains(keyCode))
				pressedKeys.insert(keyCode);

			if (releasedKeys.contains(keyCode))
				releasedKeys.erase(releasedKeys.find(keyCode));
			else
				getKeyResult = KEY_PRESSED;

		}
		else if (getKeyResult == KEY_RELEASE)
		{
			if (!releasedKeys.contains(keyCode))
				releasedKeys.insert(keyCode);

			if (pressedKeys.contains(keyCode))
				pressedKeys.erase(pressedKeys.find(keyCode));
			else
				getKeyResult = KEY_RELEASED;
		}

		return keyStatesForThisFrame[keyCode] = getKeyResult;
	}
}

int Window::GetMouseButton(int mouseButton, bool clearFrameMouseButtonStates) const
{
	static std::unordered_set<int> pressedButtons = {};
	static std::unordered_set<int> releasedButtons = {};

	static std::unordered_map<int, int> mouseButtonStatesForThisFrame;

	if (clearFrameMouseButtonStates)
	{
		mouseButtonStatesForThisFrame.clear();
		return -1;
	}

	if (mouseButtonStatesForThisFrame.find(mouseButton) != mouseButtonStatesForThisFrame.end())
	{
		return mouseButtonStatesForThisFrame[mouseButton];
	}
	else
	{
		int getMouseButtonResult = glfwGetMouseButton(glfwWindow, mouseButton);

		if (getMouseButtonResult == KEY_PRESS)
		{

			if (!pressedButtons.contains(mouseButton))
				pressedButtons.insert(mouseButton);

			if (releasedButtons.contains(mouseButton))
				releasedButtons.erase(releasedButtons.find(mouseButton));
			else
				getMouseButtonResult = KEY_PRESSED;

		}
		else if (getMouseButtonResult == KEY_RELEASE)
		{
			if (!releasedButtons.contains(mouseButton))
				releasedButtons.insert(mouseButton);

			if (pressedButtons.contains(mouseButton))
				pressedButtons.erase(pressedButtons.find(mouseButton));
			else
				getMouseButtonResult = KEY_RELEASED;
		}

		return mouseButtonStatesForThisFrame[mouseButton] = getMouseButtonResult;
	}
}

void Window::RegisterCallbackForMouseScroll(const std::string& name, std::function<void(double, double)>* const callback)
{
	mouseScrollCallbacks[glfwWindow][name] = callback;
}

void Window::DeregisterCallbackForMouseScroll(const std::string& name)
{
	mouseScrollCallbacks[glfwWindow][name] = nullptr;
}

GLFWwindow* Window::GetGLFWwindow() const
{
	return glfwWindow;
}

void Window::DisableCursor() const
{
	glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Window::EnableCursor() const
{
	glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}
