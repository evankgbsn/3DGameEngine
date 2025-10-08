#include "InputField.h"

#include "../Renderer/Texture/TextureManager.h"
#include "../Renderer/Texture/Texture.h"
#include "../Renderer/Model/ModelManager.h"
#include "../Renderer/Model/Model.h"
#include "../Renderer/Model/Vertex.h"
#include "../Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "../Renderer/GraphicsObjects/GOSprite.h"
#include "../Math/Math.h"
#include "../Renderer/Window/WindowManager.h"
#include "../Input/InputManager.h"
#include "../Renderer/Text/Text.h"
#include "../Editor/Editor.h"

InputField::InputField(const std::string& baseTextureName, const std::string& hoveredTextureName, const std::string& pressedTextureName, const glm::vec2& dimensions, const glm::vec2& position, std::function<void()>* onE, std::function<void()>* onC, bool editor) :
	onEnter(onE),
	onChange(onC),
	maxCharacterLength(20.0f),
	relativePosition(position),
	isEditor(editor)
{
	base = TextureManager::GetTexture(baseTextureName);
	hovered = TextureManager::GetTexture(hoveredTextureName);
	pressed = TextureManager::GetTexture(pressedTextureName);

	model = ModelManager::LoadModel("InputField_X" + std::to_string(dimensions.x) + "_Y" + std::to_string(dimensions.y),
		{
			Vertex({0.0f, 0.0f, 0.02f}, {}, {0.0f, 0.0f}),
			Vertex({dimensions.x, 0.0f, 0.02f}, {}, {1.0f, 0.0f}),
			Vertex({dimensions.x, dimensions.y, 0.02f}, {}, {1.0f, 1.0f}),
			Vertex({0.0f, dimensions.y, 0.02f}, {}, {0.0f, 1.0f})
		},
		{
			0,1,2,2,3,0
		});

	Window* window = WindowManager::GetWindow("Engine");

	float width = static_cast<float>(window->GetWidth());
	float height = static_cast<float>(window->GetHeight());

	float xPos = Math::ChangeRange(0.0f, 1.0f, 0.0f, width, relativePosition.x);
	float yPos = Math::ChangeRange(0.0f, 1.0f, 0.0f, height, relativePosition.y);
	
	background = GraphicsObjectManager::CreateGOSprite(model, base, {xPos, yPos});

	float textBackgroundOffset = 12.0f;

	text = new Text("", "arial", {1.0f, 1.0f, 1.0f, 1.0f}, glm::vec2(xPos, yPos) + textBackgroundOffset, 0.5f);
	text->SetPosition(glm::vec2(xPos, yPos) + textBackgroundOffset);
	text->SetZ(0.5f);


	windowResizeCallback = new std::function<void(unsigned int, unsigned int)>([this, textBackgroundOffset](unsigned int w, unsigned int h)
		{
			float width = static_cast<float>(w);
			float height = static_cast<float>(h);

			float xPos = Math::ChangeRange(0.0f, 1.0f, 0.0f, width, relativePosition.x);
			float yPos = Math::ChangeRange(0.0f, 1.0f, 0.0f, height, relativePosition.y);

			background->SetPosition({ xPos, yPos });

			text->SetPosition(glm::vec2(xPos, yPos) + textBackgroundOffset);
		});

	static unsigned int inputFieldId = 0;

	window->RegisterCallbackForWindowResize("InputField" + std::to_string(ID = inputFieldId++), windowResizeCallback);

	mousePress = new std::function<void(int)>([this](int keyCode)
		{
			Pressed();
		});

	keyPress = new std::function<void(int)>([this](int keyCode)
		{

			std::string newText = text->GetString();
			glm::vec2 pos = text->GetPosition();


			if (keyCode == KEY_BACKSPACE)
			{
				if (newText.size() > 0)
				{
					newText = std::string(newText.begin(), newText.begin() + newText.size() - 1);

					delete text;
					text = new Text(newText, "arial", { 1.0f, 1.0f, 1.0f, 1.0f }, pos, 0.5f);
					text->SetZ(0.5f);
				}

				return;
			}

			if (maxCharacterLength <= newText.size())
			{
				return;
			}

			std::string newCharacter;

			if (!Editor::ShiftPressed())
			{
				newCharacter = { static_cast<char>(std::tolower(static_cast<char>(keyCode))) };
			}
			else
			{
				newCharacter = { static_cast<char>(keyCode) };
			}

			newText += newCharacter;

			delete text;
			text = new Text(newText, "arial", { 1.0f, 1.0f, 1.0f, 1.0f }, pos, 0.5f);
			text->SetZ(0.5f);
		});

	enabled = true;
}

InputField::~InputField()
{
	Disable();

	delete keyPress;
	delete mousePress;
	delete text;
	delete windowResizeCallback;
	
	InputManager::DeregisterCallbackForMouseButtonState(KEY_PRESS, MOUSE_BUTTON_LEFT, "InputFieldPress");

	GraphicsObjectManager::Delete(background);
}

void InputField::Update()
{
	if (!Hovered())
	{

	}
}

void InputField::Enable()
{
	if (text->IsDisabled())
	{
		text->Enable();
	}

	GraphicsObjectManager::Enable(background);
}

void InputField::Disable()
{
	if (!text->IsDisabled())
	{
		text->Disable();
	}

	if (selected)
	{
		Deselect();
	}

	GraphicsObjectManager::Disable(background);
}

void InputField::SetOnEnter(std::function<void()>* onenter)
{
	onEnter = onenter;
}

void InputField::SetOnChange(std::function<void()>* onchange)
{
	onChange = onchange;
}

void InputField::Select()
{
	SetupEnterCallback();
	
	if (isEditor)
	{
		Editor::DisableCameraMovement();
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_A, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_BACKSPACE, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_SPACE, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_APOSTROPHE, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_COMMA, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_MINUS, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_PERIOD, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_SLASH, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_0, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_1, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_2, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_3, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_4, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_5, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_6, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_7, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_8, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_9, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_SEMICOLON, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_EQUAL, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_A, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_B, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_C, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_D, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_E, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_F, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_G, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_H, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_I, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_J, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_K, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_L, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_M, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_N, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_O, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_P, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_Q, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_R, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_S, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_T, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_U, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_V, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_W, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_X, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_Y, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_Z, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_LEFT_BRACKET, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_BACKSLASH, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_RIGHT_BRACKET, keyPress, "InputField");
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_GRAVE_ACCENT, keyPress, "InputField");
	}
	else
	{
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_A, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_BACKSPACE, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_SPACE, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_APOSTROPHE, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_COMMA, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_MINUS, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_PERIOD, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_SLASH, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_0, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_1, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_2, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_3, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_4, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_5, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_6, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_7, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_8, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_9, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_SEMICOLON, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_EQUAL, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_A, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_B, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_C, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_D, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_E, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_F, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_G, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_H, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_I, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_J, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_K, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_L, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_M, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_N, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_O, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_P, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_Q, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_R, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_S, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_T, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_U, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_V, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_W, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_X, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_Y, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_Z, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_LEFT_BRACKET, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_BACKSLASH, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_RIGHT_BRACKET, keyPress, "InputField");
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_GRAVE_ACCENT, keyPress, "InputField");
	}
	

	selected = true;
}

void InputField::Deselect()
{
	CleanupEnterCallback();

	if (isEditor)
	{
		Editor::EnableCameraMovement();
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_A, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_BACKSPACE, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_SPACE, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_APOSTROPHE, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_COMMA, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_MINUS, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_PERIOD, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_SLASH, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_0, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_1, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_2, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_3, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_4, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_5, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_6, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_7, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_8, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_9, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_SEMICOLON, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_EQUAL, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_A, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_B, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_C, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_D, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_E, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_F, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_G, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_H, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_I, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_J, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_K, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_L, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_M, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_N, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_O, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_P, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_Q, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_R, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_S, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_T, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_U, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_V, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_W, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_X, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_Y, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_Z, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_LEFT_BRACKET, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_BACKSLASH, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_RIGHT_BRACKET, "InputField");
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_GRAVE_ACCENT, "InputField");
	}
	else
	{
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_A, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_BACKSPACE, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_SPACE, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_APOSTROPHE, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_COMMA, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_MINUS, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_PERIOD, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_SLASH, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_0, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_1, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_2, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_3, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_4, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_5, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_6, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_7, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_8, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_9, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_SEMICOLON, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_EQUAL, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_A, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_B, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_C, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_D, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_E, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_F, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_G, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_H, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_I, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_J, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_K, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_L, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_M, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_N, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_O, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_P, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_Q, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_R, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_S, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_T, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_U, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_V, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_W, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_X, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_Y, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_Z, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_LEFT_BRACKET, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_BACKSLASH, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_RIGHT_BRACKET, "InputField");
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_GRAVE_ACCENT, "InputField");
	}
	

	selected = false;
}

bool InputField::Hovered()
{
	if (enabled)
	{
		Window* const window = WindowManager::GetWindow("Engine");
		float windowWidth = (float)window->GetWidth();
		float windowHeight = (float)window->GetHeight();

		glm::vec2 cursorPos = window->GetCursorPosition();
		cursorPos.y = windowHeight - cursorPos.y;

		if (Math::PointIn2DModel(model, glm::mat4(1.0f), background->GetProjection(), background->GetModelMat(), cursorPos, { windowWidth, windowHeight }))
		{
			if (background->GetTexture() != hovered)
			{
				background->SetTexture(hovered);
				if (isEditor)
				{
					InputManager::EditorRegisterCallbackForMouseButtonState(KEY_PRESS, MOUSE_BUTTON_LEFT, mousePress, "InputFieldPress");
				}
				else
				{
					InputManager::RegisterCallbackForMouseButtonState(KEY_PRESS, MOUSE_BUTTON_LEFT, mousePress, "InputFieldPress");
				}
			}
		
			return true;
		}
		else
		{
			if (background->GetTexture() != base)
			{
				background->SetTexture(base);

				if (isEditor)
				{
					InputManager::EditorDeregisterCallbackForMouseButtonState(KEY_PRESS, MOUSE_BUTTON_LEFT, "InputFieldPress");
				}
				else
				{
					InputManager::DeregisterCallbackForMouseButtonState(KEY_PRESS, MOUSE_BUTTON_LEFT, "InputFieldPress");
				}
			}
		}
	}

	return false;
}

bool InputField::Pressed()
{
	if (enabled)
	{
		if (background->GetTexture() != pressed)
		{
			background->SetTexture(pressed);
			if (pressFunction != nullptr)
			{
				(*pressFunction)();
			}

			Select();
		}
	}

	return true;
}

bool InputField::Selected()
{
	return selected;
}

void InputField::SetMaxCharacterLength(unsigned int length)
{
	maxCharacterLength = length;
}

const std::string& InputField::GetText() const
{
	return text->GetString();
}

void InputField::SetupEnterCallback()
{
	enterCallback = new std::function<void(int)>([this](int keyCode)
		{
			if (onEnter != nullptr)
			{
				(*onEnter)();
			}
			Deselect();
		});

	if (isEditor)
	{
		InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_ENTER, enterCallback, "InputFieldEnter");
	}
	else
	{
		InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_ENTER, enterCallback, "InputFieldEnter");
	}
}

void InputField::CleanupEnterCallback()
{
	if (isEditor)
	{
		InputManager::EditorDeregisterCallbackForKeyState(KEY_PRESS, KEY_ENTER, "InputFieldEnter");
	}
	else
	{
		InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_ENTER, "InputFieldEnter");
	}
	delete enterCallback;
}
