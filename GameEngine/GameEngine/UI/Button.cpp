#include "Button.h"

#include "../Renderer/GraphicsObjects/GOSprite.h"
#include "../Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "../Renderer/Texture/TextureManager.h"
#include "../Renderer/Texture/Texture.h"
#include "../Renderer/Model/ModelManager.h"
#include "../Math/Math.h"
#include "../Renderer/Window/WindowManager.h"
#include "../Input/InputManager.h"
#include "Sprite.h"

Button::Button(const std::string& baseTextureName, const std::string& hoveredTextureName, const std::string& pressedTextureName, const std::string& model2DName, const glm::vec2& position, std::function<void()>* press) :
	base(TextureManager::GetTexture(baseTextureName)),
	hovered(TextureManager::GetTexture(hoveredTextureName)),
	pressed(TextureManager::GetTexture(pressedTextureName)),
	model(ModelManager::GetModel(model2DName)),
	pressFunction(press),
	enabled(true),
	relativePosition(position)
{
	Window* window = WindowManager::GetWindow("Engine");

	float width = static_cast<float>(window->GetWidth());
	float height = static_cast<float>(window->GetHeight());

	sprite = new Sprite(model2DName, baseTextureName, position, {1.0f, 1.0f});
}

Button::~Button()
{
	Window* window = WindowManager::GetWindow("Engine");

	delete sprite;
	InputManager::EditorDeregisterCallbackForMouseButtonState(KEY_RELEASE, MOUSE_BUTTON_LEFT, "ButtonPress");
}

void Button::Update()
{
	if (!Hovered())
	{
		
	}
}

void Button::Disable()
{
	enabled = false;
	sprite->Disable();
}

void Button::Enable()
{
	enabled = true;
	sprite->Enable();
}

void Button::SetScale(const glm::vec2& newScale)
{
	sprite->SetScale(newScale.x, newScale.y);
}

glm::vec2 Button::GetScale() const
{
	return sprite->GetScale();
}

bool Button::Hovered()
{
	if (enabled)
	{
		Window* const window = WindowManager::GetWindow("Engine");
		float windowWidth = (float)window->GetWidth();
		float windowHeight = (float)window->GetHeight();

		glm::vec2 cursorPos = window->GetCursorPosition();
		cursorPos.y = windowHeight - cursorPos.y;

		if (sprite->Hovered())
		{
			static std::function<void(int keyCode)> mousePress = [this](int keyCode)
				{
					Pressed();
				};

			if (sprite->GetTexture() != hovered->GetName())
			{
				sprite->SetTexture(hovered->GetName());
				InputManager::EditorRegisterCallbackForMouseButtonState(KEY_PRESS, MOUSE_BUTTON_LEFT, &mousePress, "ButtonPress");
			}

			return true;
		}
		else
		{
			if (sprite->GetTexture() != base->GetName())
			{
				sprite->SetTexture(base->GetName());
				InputManager::EditorDeregisterCallbackForMouseButtonState(KEY_PRESS, MOUSE_BUTTON_LEFT, "ButtonPress");
			}
		}
	}

	return false;
}

bool Button::Pressed()
{
	if (enabled)
	{
		if (sprite->GetTexture() != pressed->GetName())
		{
			sprite->SetTexture(pressed->GetName());
			if (pressFunction != nullptr)
			{
				(*pressFunction)();
			}
		}
	}
	
	return true;
}
