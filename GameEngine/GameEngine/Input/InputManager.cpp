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

void InputManager::EditorEnqueueInputCall(std::function<void()> inputCall)
{
	if (instance != nullptr)
	{
		std::lock_guard<std::mutex> guard(instance->inputQueueMutex);
		instance->editorInputQueue.push_back(inputCall);
	}
}

void InputManager::GetCursorPosition(std::function<void(const glm::vec2&)> callback)
{
	std::function<void()> getCursorPosition = [callback]()
	{
		const Window* const mainWindow = WindowManager::GetWindow("Engine");

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
		const Window* const mainWindow = WindowManager::GetWindow("Engine");

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

void InputManager::RegisterCallbackForGamepadAxis(int axis, std::function<void(int axis, float value)>* const callback, const std::string& name)
{
	auto registerCallbackForGamepadAxis = [axis, callback, name](std::unordered_map<int, std::unordered_map<std::string, std::function<void(int axis, float value)>*>>& map, const std::string& successLog)
		{
			if (instance != nullptr)
			{
				map[axis][name] = callback;

				char stringBuffer[5] = { '\0','\0','\0','\0','\0' };
				_itoa_s(axis, stringBuffer, 10);
				Logger::Log(successLog + std::string(&stringBuffer[0]), Logger::Category::Success);
			}
			else
			{
				Logger::Log(std::string("Calling InputManager::RegisterCallbackForKeyState before InputManager::Initialize"), Logger::Category::Warning);
			}
		};

	static const std::string successLog("Registered callback for gamepad axis code ");

	std::function<void()> inputRegisterFunctionToQueue = [registerCallbackForGamepadAxis]()
		{
			if (instance != nullptr)
			{
				registerCallbackForGamepadAxis(instance->registeredGamepadAxisEvents, successLog);
			}
		};

	EnqueueInputCall(inputRegisterFunctionToQueue);
}

void InputManager::DeregisterCallbackForGamepadAxis(int axis, const std::string& name)
{
	auto deregisterCallbackForGamepadAxis = [axis, name](std::unordered_map<int, std::unordered_map<std::string, std::function<void(int axis, float value)>*>>& map, const std::string& successLog)
		{
			if (instance != nullptr)
			{
				const auto& registeredKeys = map.find(axis);
				if (registeredKeys != map.end())
				{
					const auto& registeredKeyCallback = registeredKeys->second.find(name);
					if (registeredKeyCallback != registeredKeys->second.end())
					{
						registeredKeys->second.erase(registeredKeyCallback);
					}
				}

				char stringBuffer[5] = { '\0','\0','\0','\0','\0' };
				_itoa_s(axis, stringBuffer, 10);
				Logger::Log(successLog + std::string(&stringBuffer[0]), Logger::Category::Success);
			}
			else
			{
				Logger::Log(std::string("Calling InputManager::DeregisterCallbackForKeyState before InputManager::Initialize"), Logger::Category::Warning);
			}
		};

	static const std::string successLog("Deregistered pressed callback for gamepad axis ");

	std::function<void()> inputDeregisterFunctionToQueue = [axis, deregisterCallbackForGamepadAxis]()
		{
			if (instance != nullptr)
			{
				deregisterCallbackForGamepadAxis(instance->registeredGamepadAxisEvents, successLog);
			}
		};

	EnqueueInputCall(inputDeregisterFunctionToQueue);
}

void InputManager::RegisterCallbackForGamepadButton(int state, int button, std::function<void(int button)>* const callback, const std::string& name)
{
	auto registerCallbackForGamepadButton = [button, callback, name](std::unordered_map<int, std::unordered_map<std::string, std::function<void(int button)>*>>& map, const std::string& successLog)
		{
			if (instance != nullptr)
			{
				map[button][name] = callback;

				char stringBuffer[5] = { '\0','\0','\0','\0','\0' };
				_itoa_s(button, stringBuffer, 10);
				Logger::Log(successLog + std::string(&stringBuffer[0]), Logger::Category::Success);
			}
			else
			{
				Logger::Log(std::string("Calling InputManager::RegisterCallbackForGamepadButton before InputManager::Initialize"), Logger::Category::Warning);
			}
		};

	static const std::string pressedSuccessLog("Registered pressed callback for gamepad button ");
	static const std::string pressSuccessLog("Registered press callback for gamepad button ");
	static const std::string releaseSuccessLog("Registered release callback for gamepad button ");
	static const std::string releasedSuccessLog("Registered released callback for gamepad button ");

	std::function<void()> inputRegisterFunctionToQueue = [state, registerCallbackForGamepadButton]()
		{
			if (instance != nullptr)
			{
				switch (state)
				{
				case KEY_PRESS:
					registerCallbackForGamepadButton(instance->registeredGamepadButtonPressEvents, pressSuccessLog);
					break;
				case KEY_RELEASE:
					registerCallbackForGamepadButton(instance->registeredGamepadButtonReleaseEvents, releaseSuccessLog);
					break;
				case KEY_PRESSED:
					registerCallbackForGamepadButton(instance->registeredGamepadButtonPressedEvents, pressedSuccessLog);
					break;
				case KEY_RELEASED:
					registerCallbackForGamepadButton(instance->registeredGamepadButtonReleasedEvents, releasedSuccessLog);
					break;
				default:
					break;
				}
			}
		};

	EnqueueInputCall(inputRegisterFunctionToQueue);
}

void InputManager::DeregisterCallbackForGamepadButton(int state, int button, const std::string& name)
{
	auto deregisterCallbackForGamepadButton = [button, name](std::unordered_map<int, std::unordered_map<std::string, std::function<void(int button)>*>>& map, const std::string& successLog)
		{
			if (instance != nullptr)
			{
				const auto& registeredKeys = map.find(button);
				if (registeredKeys != map.end())
				{
					const auto& registeredKeyCallback = registeredKeys->second.find(name);
					if (registeredKeyCallback != registeredKeys->second.end())
					{
						registeredKeys->second.erase(registeredKeyCallback);
					}
				}

				char stringBuffer[5] = { '\0','\0','\0','\0','\0' };
				_itoa_s(button, stringBuffer, 10);
				Logger::Log(successLog + std::string(&stringBuffer[0]), Logger::Category::Success);
			}
			else
			{
				Logger::Log(std::string("Calling InputManager::DeregisterCallbackForGamepadButton before InputManager::Initialize"), Logger::Category::Warning);
			}
		};

	static const std::string pressedSuccessLog("Deregistered pressed callback for gamepadButton ");
	static const std::string pressSuccessLog("Deregistered press callback for gamepadButton ");
	static const std::string releaseSuccessLog("Deregistered release callback for gamepadButton ");

	std::function<void()> inputDeregisterFunctionToQueue = [state, deregisterCallbackForGamepadButton]()
		{
			if (instance != nullptr)
			{
				switch (state)
				{
				case KEY_PRESS:
					deregisterCallbackForGamepadButton(instance->registeredGamepadButtonPressEvents, pressSuccessLog);
					break;
				case KEY_RELEASE:
					deregisterCallbackForGamepadButton(instance->registeredGamepadButtonReleaseEvents, releaseSuccessLog);
					break;
				case KEY_PRESSED:
					deregisterCallbackForGamepadButton(instance->registeredGamepadButtonPressedEvents, releaseSuccessLog);
					break;
				case KEY_RELEASED:
					deregisterCallbackForGamepadButton(instance->registeredGamepadButtonReleasedEvents, releaseSuccessLog);
					break;
				default:
					break;
				}
			}
		};

	EnqueueInputCall(inputDeregisterFunctionToQueue);
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

void InputManager::ProcessGamepadButtonEvents() const
{
	const Window* const mainWindow = WindowManager::GetWindow(std::string("Engine"));
	std::function<void(int state, const std::unordered_map<int, std::unordered_map<std::string, std::function<void(int button)>*>>&)> processButtonEvents = [mainWindow](int state, const std::unordered_map<int, std::unordered_map<std::string, std::function<void(int button)>*>>& map)
		{
			if (instance != nullptr)
			{
				for (const auto& buttonCallbacks : map)
				{
					int keyState = mainWindow->GetGamepadButton(buttonCallbacks.first);

					if (keyState == state)
					{
						for (const auto& func : buttonCallbacks.second)
						{
							if (func.second != nullptr)
							{
								(*func.second)(buttonCallbacks.first);
							}
						}
					}
				}
			}
		};

	if (instance != nullptr)
	{
		processButtonEvents(KEY_RELEASE, instance->registeredGamepadButtonReleaseEvents);
		processButtonEvents(KEY_PRESS, instance->registeredGamepadButtonPressEvents);
		processButtonEvents(KEY_PRESSED, instance->registeredGamepadButtonPressedEvents);
		processButtonEvents(KEY_RELEASED, instance->registeredGamepadButtonReleasedEvents);

		mainWindow->GetGamepadButton(GAMEPAD_BUTTON_A, true);
	}
}

void InputManager::ProcessGamepadAxisEvents() const
{
	if (instance != nullptr)
	{
		if (instance != nullptr)
		{
			for (const auto& axisCallbacks : instance->registeredGamepadAxisEvents)
			{
				for (const auto& func : axisCallbacks.second)
				{
					if (func.second != nullptr)
					{
						GLFWgamepadstate state;

						if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state))
						{
							(*func.second)(axisCallbacks.first, state.axes[axisCallbacks.first]);
						}
					}
				}
			}
		}
	}
}

void InputManager::EditorProcessKeyEvents() const
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
		processKeyEvents(KEY_RELEASE, instance->editorRegisteredKeyReleaseEvents);
		processKeyEvents(KEY_PRESS, instance->editorRegisteredKeyPressEvents);
		processKeyEvents(KEY_PRESSED, instance->editorRegisteredKeyPressedEvents);
		processKeyEvents(KEY_RELEASED, instance->editorRegisteredKeyReleasedEvents);

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

void InputManager::EditorProcessMouseButtonEvents() const
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
		processMouseButtonEvents(KEY_RELEASE, instance->editorRegisteredMouseButtonReleaseEvents);
		processMouseButtonEvents(KEY_PRESS, instance->editorRegisteredMouseButtonPressEvents);
		processMouseButtonEvents(KEY_PRESSED, instance->editorRegisteredMouseButtonPressedEvents);
		processMouseButtonEvents(KEY_RELEASED, instance->editorRegisteredMouseButtonReleasedEvents);

		mainWindow->GetMouseButton(MOUSE_BUTTON_1, true);
	}
}

void InputManager::SetupController()
{
	int present = glfwJoystickPresent(GLFW_JOYSTICK_1);

	if (GLFW_TRUE == present)
	{
		if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1))
		{
			const char* name = glfwGetGamepadName(GLFW_JOYSTICK_1);
			std::string controllerName = name;
			controllerName += " ";
		}
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
					deregisterCallbackForMouseButtonState(instance->registeredMouseButtonPressEvents, pressSuccessLog);
					break;
				case KEY_RELEASE:
					deregisterCallbackForMouseButtonState(instance->registeredMouseButtonReleaseEvents, releaseSuccessLog);
					break;
				case KEY_PRESSED:
					deregisterCallbackForMouseButtonState(instance->registeredMouseButtonPressedEvents, releaseSuccessLog);
					break;
				case KEY_RELEASED:
					deregisterCallbackForMouseButtonState(instance->registeredMouseButtonReleasedEvents, releaseSuccessLog);
					break;
				default:
					break;
				}
			}
		};

	EnqueueInputCall(inputDeregisterFunctionToQueue);
}

void InputManager::RegisterCallbackForMouseScroll(std::function<void(double, double)>* const callback, const std::string& name)
{
	EnqueueInputCall(std::function<void()>([callback, name]()
		{
			WindowManager::GetWindow("Engine")->RegisterCallbackForMouseScroll(name, callback);
		}));
}

void InputManager::DeregisterCallbackForMouseScroll(const std::string& name)
{
	EnqueueInputCall(std::function<void()>([name]()
		{
			WindowManager::GetWindow("Engine")->DeregisterCallbackForMouseScroll(name);
		}));
}

void InputManager::EditorRegisterCallbackForKeyState(int state, int keyCode, std::function<void(int keyCode)>* const callback, const std::string& name)
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
					registerCallbackForKeyState(instance->editorRegisteredKeyPressEvents, pressSuccessLog);
					break;
				case KEY_RELEASE:
					registerCallbackForKeyState(instance->editorRegisteredKeyReleaseEvents, releaseSuccessLog);
					break;
				case KEY_PRESSED:
					registerCallbackForKeyState(instance->editorRegisteredKeyPressedEvents, pressedSuccessLog);
					break;
				case KEY_RELEASED:
					registerCallbackForKeyState(instance->editorRegisteredKeyReleasedEvents, releasedSuccessLog);
					break;
				default:
					break;
				}
			}
		};

	EditorEnqueueInputCall(inputRegisterFunctionToQueue);
}

void InputManager::EditorDeregisterCallbackForKeyState(int state, int keyCode, const std::string& name)
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
					deregisterCallbackForKeyState(instance->editorRegisteredKeyPressEvents, pressSuccessLog);
					break;
				case KEY_RELEASE:
					deregisterCallbackForKeyState(instance->editorRegisteredKeyReleaseEvents, releaseSuccessLog);
					break;
				case KEY_PRESSED:
					deregisterCallbackForKeyState(instance->editorRegisteredKeyPressedEvents, releaseSuccessLog);
					break;
				case KEY_RELEASED:
					deregisterCallbackForKeyState(instance->editorRegisteredKeyReleasedEvents, releaseSuccessLog);
					break;
				default:
					break;
				}
			}
		};

	EditorEnqueueInputCall(inputDeregisterFunctionToQueue);
}

void InputManager::EditorRegisterCallbackForMouseButtonState(int state, int mouseButton, std::function<void(int mouseButton)>* const callback, const std::string& name)
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
					registerCallbackForMouseButtonState(instance->editorRegisteredMouseButtonPressEvents, pressSuccessLog);
					break;
				case KEY_RELEASE:
					registerCallbackForMouseButtonState(instance->editorRegisteredMouseButtonReleaseEvents, releaseSuccessLog);
					break;
				case KEY_PRESSED:
					registerCallbackForMouseButtonState(instance->editorRegisteredMouseButtonPressedEvents, pressedSuccessLog);
					break;
				case KEY_RELEASED:
					registerCallbackForMouseButtonState(instance->editorRegisteredMouseButtonReleasedEvents, releasedSuccessLog);
					break;
				default:
					break;
				}
			}
		};

	EditorEnqueueInputCall(inputRegisterFunctionToQueue);
}

void InputManager::EditorDeregisterCallbackForMouseButtonState(int state, int mouseButton, const std::string& name)
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
					deregisterCallbackForMouseButtonState(instance->editorRegisteredMouseButtonPressEvents, pressSuccessLog);
					break;
				case KEY_RELEASE:
					deregisterCallbackForMouseButtonState(instance->editorRegisteredMouseButtonReleaseEvents, releaseSuccessLog);
					break;
				case KEY_PRESSED:
					deregisterCallbackForMouseButtonState(instance->editorRegisteredMouseButtonPressedEvents, releaseSuccessLog);
					break;
				case KEY_RELEASED:
					deregisterCallbackForMouseButtonState(instance->editorRegisteredMouseButtonReleasedEvents, releaseSuccessLog);
					break;
				default:
					break;
				}
			}
		};

	EditorEnqueueInputCall(inputDeregisterFunctionToQueue);
}

void InputManager::EditorGetCursorPosition(std::function<void(const glm::vec2& cursorPositon)> callback)
{
	std::function<void()> getCursorPosition = [callback]()
		{
			const Window* const mainWindow = WindowManager::GetWindow("Engine");

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

	EditorEnqueueInputCall(getCursorPosition);
}

void InputManager::EditorWhenCursorMoved(std::function<void(const glm::vec2& newCursorPosition)> callback)
{
	std::function<void()> getCursorPosition = [callback]()
		{
			const Window* const mainWindow = WindowManager::GetWindow("Engine");

			if (mainWindow != nullptr)
			{
				glm::vec2 newCursorPosition;
				bool moved = mainWindow->GetCursorMoved(newCursorPosition);

				if (moved)
					callback(newCursorPosition);
			}
			else
			{
				Logger::Log(std::string("Failed to get cursor position. Could not get window reference. InputManager::GetCursorPosition"), Logger::Category::Warning);
				callback(glm::vec2(0.0f, 0.0f));
			}
		};

	EditorEnqueueInputCall(getCursorPosition);
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
		instance->ProcessGamepadAxisEvents();
		instance->ProcessGamepadButtonEvents();
	}
	else
	{
		Logger::Log(std::string("Calling InputManager::Update before InputManager::Initialize"), Logger::Category::Warning);
	}
}

void InputManager::EditorUpdate()
{
	if (instance != nullptr)
	{
		if (!instance->editorInputQueue.empty())
		{
			for (std::function<void()> func : instance->editorInputQueue)
			{
				func();
			}

			std::list<std::function<void()>>::iterator it = instance->editorInputQueue.begin();
			for (it; it != instance->editorInputQueue.end();)
			{
				it = instance->editorInputQueue.erase(it);
			}
		}

		instance->EditorProcessKeyEvents();
		instance->EditorProcessMouseButtonEvents();
	}
	else
	{
		Logger::Log(std::string("Calling InputManager::EditorUpdate before InputManager::Initialize"), Logger::Category::Warning);
	}
}

void InputManager::DisableCursor(const std::string& window)
{
	Window* windowPtr = WindowManager::GetWindow(window);

	if (windowPtr != nullptr)
	{
		windowPtr->DisableCursor();
	}
}

void InputManager::EnableCursor(const std::string& window)
{
	Window* windowPtr = WindowManager::GetWindow(window);

	if (windowPtr != nullptr)
	{
		windowPtr->EnableCursor();
	}
}

InputManager::InputManager() :
	inputQueue(std::list<std::function<void()>>()),
	keysPressed(std::unordered_set<int>())
{
	SetupController();
}

InputManager::~InputManager()
{
	instance = nullptr;
}
