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
	glfwWindow = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), name.c_str(), nullptr, nullptr);

	if (glfwWindow == nullptr)
	{
		Logger::Log("Failed to create window " + name, Logger::Category::Error);
	}
	else
	{
		glfwMakeContextCurrent(glfwWindow);
		InitializeGLEW();
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
