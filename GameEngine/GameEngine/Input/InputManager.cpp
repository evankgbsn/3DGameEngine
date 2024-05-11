#include "InputManager.h"

#include "../Utils/Logger.h"

#include "../Renderer/Window/WindowManager.h"
#include "../Renderer/Window/Window.h"

#include <GLFW/glfw3.h>

InputManager* InputManager::instance = nullptr;

void InputManager::Initialize()
{
	if (instance == nullptr)
	{
		instance = new InputManager();
		Logger::Log(std::string("Initialized InputManager"), Logger::Category::Success);
	}
	else
	{
		Logger::Log(std::string("Calling InputManager::Initialize before InputManager::Terminate"), Logger::Category::Warning);
	}
}

void InputManager::Terminate()
{
	if (instance != nullptr)
	{
		delete instance;
		Logger::Log(std::string("Terminated InputManager"), Logger::Category::Success);
	}
	else
	{
		Logger::Log(std::string("Calling InputManager::Terminate before InputManager::Initialize"), Logger::Category::Warning);
	}
}

void InputManager::EnqueueInputCall(std::function<void()> inputCall)
{
	if (instance != nullptr)
	{
		std::lock_guard<std::mutex> guard(instance->inputQueueMutex);
		instance->inputQueue.push_back(inputCall);
	}
}

void InputManager::GetCursorPosition(std::function<void(const glm::vec2&)> callback)
{
	std::function<void()> getCursorPosition = [callback]()
	{
		const Window* const mainWindow = WindowManager::GetWindow("MainWindow");

		if (mainWindow != nullptr)
		{
			callback(mainWindow->GetCursorPosition());
		}
		else
		{
			Logger::Log(std::string("Failed to get cursor position. Could not get window reference. InputManager::GetCursorPosition"), Logger::Category::Warning);
			callback(glm::vec2(0.0f, 0.0f));
		}
	};
	
	EnqueueInputCall(getCursorPosition);
}

void InputManager::WhenCursorMoved(std::function<void(const glm::vec2& newCursorPosition)> callback)
{
	std::function<void()> getCursorPosition = [callback]()
	{
		const Window* const mainWindow = WindowManager::GetWindow("MainWindow");

		if (mainWindow != nullptr)
		{
			glm::vec2 newCursorPosition;
			bool moved = mainWindow->GetCursorMoved(newCursorPosition);
			
			if(moved) 
				callback(newCursorPosition);
		}
		else
		{
			Logger::Log(std::string("Failed to get cursor position. Could not get window reference. InputManager::GetCursorPosition"), Logger::Category::Warning);
			callback(glm::vec2(0.0f, 0.0f));
		}
	};

	EnqueueInputCall(getCursorPosition);
}

void InputManager::RegisterCallbackForKeyState(int state, int keyCode, std::function<void(int keyCode)>* callback, const std::string& name)
{
	auto registerCallbackForKeyState = [keyCode, callback, name](std::unordered_map<int, std::unordered_map<std::string, std::function<void(int keyCode)>*>>& map, const std::string& successLog)
	{
		if (instance != nullptr)
		{
			map[keyCode][name] = callback;

			char stringBuffer[5] = { '\0','\0','\0','\0','\0' };
			_itoa_s(keyCode, stringBuffer, 10);
			Logger::Log(successLog + std::string(&stringBuffer[0]), Logger::Category::Success);
		}
		else
		{
			Logger::Log(std::string("Calling InputManager::RegisterCallbackForKeyState before InputManager::Initialize"), Logger::Category::Warning);
		}
	};

	static const std::string pressedSuccessLog("Registered pressed callback for key code ");
	static const std::string pressSuccessLog("Registered press callback for key code ");
	static const std::string releaseSuccessLog("Registered release callback for key code ");
	static const std::string releasedSuccessLog("Registered released callback for key code ");

	std::function<void()> inputRegisterFunctionToQueue = [state, registerCallbackForKeyState]()
	{
		if (instance != nullptr)
		{
			switch (state)
			{
			case KEY_PRESS:
				registerCallbackForKeyState(instance->registeredKeyPressEvents, pressSuccessLog);
				break;
			case KEY_RELEASE:
				registerCallbackForKeyState(instance->registeredKeyReleaseEvents, releaseSuccessLog);
				break;
			case KEY_PRESSED:
				registerCallbackForKeyState(instance->registeredKeyPressedEvents, pressedSuccessLog);
				break;
			case KEY_RELEASED:
				registerCallbackForKeyState(instance->registeredKeyReleasedEvents, releasedSuccessLog);
				break;
			default:
				break;
			}
		}
	};

	EnqueueInputCall(inputRegisterFunctionToQueue);
}

void InputManager::DeregisterCallbackForKeyState(int state, int keyCode, const std::string& name)
{
	auto deregisterCallbackForKeyState = [keyCode, name](std::unordered_map<int, std::unordered_map<std::string, std::function<void(int keyCode)>*>>& map, const std::string& successLog)
	{
		if (instance != nullptr)
		{
			const auto& registeredKeys = map.find(keyCode);
			if (registeredKeys != map.end())
			{
				const auto& registeredKeyCallback = registeredKeys->second.find(name);
				if (registeredKeyCallback != registeredKeys->second.end())
				{
					registeredKeys->second.erase(registeredKeyCallback);
				}
			}

			char stringBuffer[5] = { '\0','\0','\0','\0','\0' };
			_itoa_s(keyCode, stringBuffer, 10);
			Logger::Log(successLog + std::string(&stringBuffer[0]), Logger::Category::Success);
		}
		else
		{
			Logger::Log(std::string("Calling InputManager::DeregisterCallbackForKeyState before InputManager::Initialize"), Logger::Category::Warning);
		}
	};

	static const std::string pressedSuccessLog("Deregistered pressed callback for key code ");
	static const std::string pressSuccessLog("Deregistered press callback for key code ");
	static const std::string releaseSuccessLog("Deregistered release callback for key code ");

	std::function<void()> inputDeregisterFunctionToQueue = [state, deregisterCallbackForKeyState]()
	{
		if (instance != nullptr)
		{
			switch (state)
			{
			case KEY_PRESS:
				deregisterCallbackForKeyState(instance->registeredKeyPressEvents, pressSuccessLog);
				break;
			case KEY_RELEASE:
				deregisterCallbackForKeyState(instance->registeredKeyReleaseEvents, releaseSuccessLog);
				break;
			case KEY_PRESSED:
				deregisterCallbackForKeyState(instance->registeredKeyPressedEvents, releaseSuccessLog);
				break;
			case KEY_RELEASED:
				deregisterCallbackForKeyState(instance->registeredKeyReleasedEvents, releaseSuccessLog);
				break;
			default:
				break;
			}
		}
	};

	EnqueueInputCall(inputDeregisterFunctionToQueue); 
}

void InputManager::RegisterCallbackForMouseButtonState(int state, int mouseButton, std::function<void(int mouseButton)>* const callback, const std::string& name)
{
	auto registerCallbackForMouseButtonState = [mouseButton, callback, name](std::unordered_map<int, std::unordered_map<std::string, std::function<void(int mouseButton)>*>>& map, const std::string& successLog)
		{
			if (instance != nullptr)
			{
				map[mouseButton][name] = callback;

				char stringBuffer[5] = { '\0','\0','\0','\0','\0' };
				_itoa_s(mouseButton, stringBuffer, 10);
				Logger::Log(successLog + std::string(&stringBuffer[0]), Logger::Category::Success);
			}
			else
			{
				Logger::Log(std::string("Calling InputManager::RegisterCallbackForKeyState before InputManager::Initialize"), Logger::Category::Warning);
			}
		};

	static const std::string pressedSuccessLog("Registered pressed callback mouse button ");
	static const std::string pressSuccessLog("Registered press callback mouse button ");
	static const std::string releaseSuccessLog("Registered release callback mouse button ");
	static const std::string releasedSuccessLog("Registered released callback mouse button ");

	std::function<void()> inputRegisterFunctionToQueue = [state, registerCallbackForMouseButtonState]()
		{
			if (instance != nullptr)
			{
				switch (state)
				{
				case KEY_PRESS:
					registerCallbackForMouseButtonState(instance->registeredMouseButtonPressEvents, pressSuccessLog);
					break;
				case KEY_RELEASE:
					registerCallbackForMouseButtonState(instance->registeredMouseButtonReleaseEvents, releaseSuccessLog);
					break;
				case KEY_PRESSED:
					registerCallbackForMouseButtonState(instance->registeredMouseButtonPressedEvents, pressedSuccessLog);
					break;
				case KEY_RELEASED:
					registerCallbackForMouseButtonState(instance->registeredMouseButtonReleasedEvents, releasedSuccessLog);
					break;
				default:
					break;
				}
			}
		};

	EnqueueInputCall(inputRegisterFunctionToQueue);
}

void InputManager::ProcessKeyEvents() const
{
	const Window* const mainWindow = WindowManager::GetWindow(std::string("Engine"));
	std::function<void(int state, const std::unordered_map<int, std::unordered_map<std::string, std::function<void(int keyCode)>*>>&)> processKeyEvents = [mainWindow](int state, const std::unordered_map<int, std::unordered_map<std::string, std::function<void(int keyCode)>*>>& map)
	{
		if (instance != nullptr)
		{
			for (const auto& keyCallbacks : map)
			{
				int keyState = mainWindow->GetKey(keyCallbacks.first);

				if (keyState == state)
				{
					for (const auto& func : keyCallbacks.second)
					{
						if (func.second != nullptr)
						{
							(*func.second)(keyCallbacks.first);
						}
					}
				}
			}
		}
	};

	if (instance != nullptr)
	{
		processKeyEvents(KEY_RELEASE, instance->registeredKeyReleaseEvents);
		processKeyEvents(KEY_PRESS, instance->registeredKeyPressEvents);
		processKeyEvents(KEY_PRESSED, instance->registeredKeyPressedEvents);
		processKeyEvents(KEY_RELEASED, instance->registeredKeyReleasedEvents);

		mainWindow->GetKey(KEY_0, true);
	}
}

void InputManager::ProcessMouseButtonEvents() const
{
	const Window* const mainWindow = WindowManager::GetWindow(std::string("Engine"));
	std::function<void(int state, const std::unordered_map<int, std::unordered_map<std::string, std::function<void(int mouseButton)>*>>&)> processMouseButtonEvents = [mainWindow](int state, const std::unordered_map<int, std::unordered_map<std::string, std::function<void(int mouseButton)>*>>& map)
		{
			if (instance != nullptr)
			{
				for (const auto& mouseButtonCallbacks : map)
				{
					int mouseButtonState = mainWindow->GetMouseButton(mouseButtonCallbacks.first);

					if (mouseButtonState == state)
					{
						for (const auto& func : mouseButtonCallbacks.second)
						{
							if (func.second != nullptr)
							{
								(*func.second)(mouseButtonCallbacks.first);
							}
						}
					}
				}
			}
		};

	if (instance != nullptr)
	{
		processMouseButtonEvents(KEY_RELEASE, instance->registeredMouseButtonReleaseEvents);
		processMouseButtonEvents(KEY_PRESS, instance->registeredMouseButtonPressEvents);
		processMouseButtonEvents(KEY_PRESSED, instance->registeredMouseButtonPressedEvents);
		processMouseButtonEvents(KEY_RELEASED, instance->registeredMouseButtonReleasedEvents);

		mainWindow->GetMouseButton(MOUSE_BUTTON_1, true);
	}
}

void InputManager::DeregisterCallbackForMouseButtonState(int state, int mouseButton, const std::string& name)
{
	auto deregisterCallbackForMouseButtonState = [mouseButton, name](std::unordered_map<int, std::unordered_map<std::string, std::function<void(int mouseButton)>*>>& map, const std::string& successLog)
		{
			if (instance != nullptr)
			{
				const auto& registeredKeys = map.find(mouseButton);
				if (registeredKeys != map.end())
				{
					const auto& registeredKeyCallback = registeredKeys->second.find(name);
					if (registeredKeyCallback != registeredKeys->second.end())
					{
						registeredKeys->second.erase(registeredKeyCallback);
					}
				}

				char stringBuffer[5] = { '\0','\0','\0','\0','\0' };
				_itoa_s(mouseButton, stringBuffer, 10);
				Logger::Log(successLog + std::string(&stringBuffer[0]), Logger::Category::Success);
			}
			else
			{
				Logger::Log(std::string("Calling InputManager::DeregisterCallbackForKeyState before InputManager::Initialize"), Logger::Category::Warning);
			}
		};

	static const std::string pressedSuccessLog("Deregistered pressed callback for mouse button ");
	static const std::string pressSuccessLog("Deregistered press callback for mouse button ");
	static const std::string releaseSuccessLog("Deregistered release callback for mouse button ");

	std::function<void()> inputDeregisterFunctionToQueue = [state, deregisterCallbackForMouseButtonState]()
		{
			if (instance != nullptr)
			{
				switch (state)
				{
				case KEY_PRESS:
					deregisterCallbackForMouseButtonState(instance->registeredKeyPressEvents, pressSuccessLog);
					break;
				case KEY_RELEASE:
					deregisterCallbackForMouseButtonState(instance->registeredKeyReleaseEvents, releaseSuccessLog);
					break;
				case KEY_PRESSED:
					deregisterCallbackForMouseButtonState(instance->registeredKeyPressedEvents, releaseSuccessLog);
					break;
				case KEY_RELEASED:
					deregisterCallbackForMouseButtonState(instance->registeredKeyReleasedEvents, releaseSuccessLog);
					break;
				default:
					break;
				}
			}
		};

	EnqueueInputCall(inputDeregisterFunctionToQueue);
}

void InputManager::Update()
{
	if (instance != nullptr)
	{
		if (!instance->inputQueue.empty())
		{
			for (std::function<void()> func : instance->inputQueue)
			{
				func();
			}

			std::list<std::function<void()>>::iterator it = instance->inputQueue.begin();
			for (it; it != instance->inputQueue.end();)
			{
				it = instance->inputQueue.erase(it);
			}
		}
		
		instance->ProcessKeyEvents();
		instance->ProcessMouseButtonEvents();
	}
	else
	{
		Logger::Log(std::string("Calling InputManager::Update before InputManager::Initialize"), Logger::Category::Warning);
	}
}

InputManager::InputManager() :
	inputQueue(std::list<std::function<void()>>()),
	keysPressed(std::unordered_set<int>())
{

}

InputManager::~InputManager()
{
	instance = nullptr;
}
