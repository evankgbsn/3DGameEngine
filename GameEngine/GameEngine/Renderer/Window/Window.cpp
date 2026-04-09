#include "Window.h"

#include "../../Utils/Logger.h"
#include "../Shader/ShaderManager.h"
#include "../Model/ModelManager.h"
#include "../Model/Model.h"
#include "../GraphicsObjects/GraphicsObjectManager.h"
#include "../Texture/TextureManager.h"
#include "../../Input/InputManager.h"
#include "../../Editor/Editor.h"
#include "../Renderer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

std::unordered_map<GLFWwindow*, std::unordered_map<std::string, std::function<void(double, double)>*>> Window::mouseScrollCallbacks = std::unordered_map<GLFWwindow*, std::unordered_map<std::string, std::function<void(double, double)>*>>();

std::unordered_map<GLFWwindow*, std::unordered_map<std::string, std::function<void(unsigned int, unsigned int)>*>> Window::windowResizeCallbacks = std::unordered_map<GLFWwindow*, std::unordered_map<std::string, std::function<void(unsigned int, unsigned int)>*>>();

void WindowScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	for (auto& function : Window::mouseScrollCallbacks[window])
	{
		(*function.second)(xoffset, yoffset);
	}
}

void WindowResizeCallback(GLFWwindow* window, int width, int height)
{
	for (auto& function : Window::windowResizeCallbacks[window])
	{
		(*function.second)(static_cast<unsigned int>(width), static_cast<unsigned int>(height));
	}
}

unsigned int Window::GetWidth() const
{
	return dimensions.x;
}

unsigned int Window::GetHeight() const
{
	return dimensions.y;
}

Window::Window(unsigned int width, unsigned int height, const std::string& name) :
	glfwWindow(nullptr),
	shouldClose(false),
	dimensions({width, height}),
	windowResizeCallback(nullptr)
{
	int monitorCount;
	GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
	glfwWindow = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), name.c_str(), nullptr, nullptr);
	glfwGetWindowPos(glfwWindow, &position.x, &position.y);

	if (glfwWindow == nullptr)
	{
		Logger::Log("Failed to create window " + name, Logger::Category::Error);
	}
	else
	{
		 windowResizeCallback = new std::function<void(unsigned int, unsigned int)>([this](unsigned int width, unsigned int height)
			{
				 dimensions = { width, height };
			});

		RegisterCallbackForWindowResize("WindowResizeDimensionsUpdate", windowResizeCallback);

		glfwMakeContextCurrent(glfwWindow);
		glfwSwapInterval(0);
		InitializeGLEW();
		glfwSetScrollCallback(glfwWindow, WindowScrollCallback);
		glfwSetWindowSizeCallback(glfwWindow, WindowResizeCallback);
	}
}

Window::~Window()
{
	if (windowResizeCallback != nullptr)
	{
		delete windowResizeCallback;
	}

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
		if (Editor::IsEnabled())
		{
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		}
		else
		{
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		}
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
			{
				pressedKeys.insert(keyCode);
			}

			if (releasedKeys.contains(keyCode))
			{
				releasedKeys.erase(releasedKeys.find(keyCode));
			}
			else
			{
				getKeyResult = KEY_PRESSED;
			}

		}
		else if (getKeyResult == KEY_RELEASE)
		{
			if (!releasedKeys.contains(keyCode))
			{
				releasedKeys.insert(keyCode);
			}

			if (pressedKeys.contains(keyCode))
			{
				pressedKeys.erase(pressedKeys.find(keyCode));
			}
			else
			{
				getKeyResult = KEY_RELEASED;
			}
		}

		return keyStatesForThisFrame[keyCode] = getKeyResult;
	}
}

int Window::GetGamepadButton(int button, bool clearFrameKeyStates) const
{
	static std::unordered_set<int> pressedButtons = {};
	static std::unordered_set<int> releasedButtons = {};

	static std::unordered_map<int, int> buttonStatesForThisFrame;

	if (clearFrameKeyStates)
	{
		buttonStatesForThisFrame.clear();
		return -1;
	}

	if (buttonStatesForThisFrame.find(button) != buttonStatesForThisFrame.end())
	{
		return buttonStatesForThisFrame[button];
	}
	else
	{
		GLFWgamepadstate state;

		if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state))
		{
			int getKeyResult = state.buttons[GLFW_GAMEPAD_BUTTON_A];

			if (getKeyResult == KEY_PRESS)
			{

				if (!pressedButtons.contains(button))
					pressedButtons.insert(button);

				if (releasedButtons.contains(button))
					releasedButtons.erase(releasedButtons.find(button));
				else
					getKeyResult = KEY_PRESSED;

			}
			else if (getKeyResult == KEY_RELEASE)
			{
				if (!releasedButtons.contains(button))
					releasedButtons.insert(button);

				if (pressedButtons.contains(button))
					pressedButtons.erase(pressedButtons.find(button));
				else
					getKeyResult = KEY_RELEASED;
			}

			return buttonStatesForThisFrame[button] = getKeyResult;
		}
		else
		{
			return -1;
		}
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
	mouseScrollCallbacks[glfwWindow].erase(mouseScrollCallbacks[glfwWindow].find(name));
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

void Window::RegisterCallbackForWindowResize(const std::string& name, std::function<void(unsigned int width, unsigned int height)>* const callback)
{
	windowResizeCallbacks[glfwWindow][name] = callback;
}

void Window::DeregisterCallbackForWindowResize(const std::string& name)
{
	windowResizeCallbacks[glfwWindow].erase(windowResizeCallbacks[glfwWindow].find(name));
}

std::string Window::GetTitle() const
{
	return std::string(glfwGetWindowTitle(glfwWindow));
}

void Window::SetTitle(const std::string& newTitle)
{
	glfwSetWindowTitle(glfwWindow, newTitle.c_str());
}

glm::vec2 Window::GetPrimaryMonitorDimensions()
{
	glm::vec2 dimensions = { 0.0f, 0.0f };

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();

	if (monitor != nullptr)
	{
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		if (mode != nullptr)
		{
			dimensions.x = mode->width;
			dimensions.y = mode->height;
		}
	}

	return dimensions;
}

void Window::ToggleFullScreen()
{
	// If glfwGetWindowMonitor returns a non-null pointer, the window is currently full screen
	if (glfwGetWindowMonitor(glfwWindow))
	{
		// Switch back to windowed mode using the saved position and size
		glm::vec2 primaryMonitorDimensions = Renderer::GetPrimaryMonitorDimensions();

		glfwSetWindowMonitor(glfwWindow, NULL, position.x, position.y, static_cast<unsigned int>(primaryMonitorDimensions.x / 2), static_cast<unsigned int>(primaryMonitorDimensions.y / 2), 0);
		isFullScreen = false;
	}
	else
	{
		// Window is currently windowed. Save the current position and size!
		glfwGetWindowPos(glfwWindow, &position.x, &position.y);
		glfwGetWindowSize(glfwWindow, &dimensions.x, &dimensions.y);

		// Get the primary monitor and its video mode
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		// Switch to full screen
		// Note: Passing mode->refreshRate attempts to set exclusive full screen. 
		// Passing GLFW_DONT_CARE for refresh rate usually gives borderless windowed.
		glfwSetWindowMonitor(glfwWindow, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
		isFullScreen = true;
	}
}
