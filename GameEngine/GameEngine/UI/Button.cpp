#include "Button.h"

#include "../Renderer/GraphicsObjects/GOSprite.h"
#include "../Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "../Renderer/Texture/TextureManager.h"
#include "../Renderer/Model/ModelManager.h"
#include "../Math/Math.h"
#include "../Renderer/Window/WindowManager.h"
#include "../Input/InputManager.h"

Button::Button(const std::string& baseTextureName, const std::string& hoveredTextureName, const std::string& pressedTextureName, const std::string& model2DName, const glm::vec2& position, std::function<void()>* press) :
	base(TextureManager::GetTexture(baseTextureName)),
	hovered(TextureManager::GetTexture(hoveredTextureName)),
	pressed(TextureManager::GetTexture(pressedTextureName)),
	model(ModelManager::GetModel(model2DName)),
	pressFunction(press),
	enabled(true)
{
	sprite = GraphicsObjectManager::CreateGOSprite(model, base, position);
}

Button::~Button()
{
	
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
	GraphicsObjectManager::Disable(sprite);
}

void Button::Enable()
{
	enabled = true;
	GraphicsObjectManager::Enable(sprite);
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

		glm::mat4 transform(1.0f);

		transform[3] = glm::vec4(sprite->GetPosition(), 0.0f, 1.0f);

		if (Math::PointIn2DModel(model, transform, sprite->GetProjection(), glm::mat4(1.0f), cursorPos, { windowWidth, windowHeight }))
		{
			static std::function<void(int keyCode)> mousePress = [this](int keyCode)
				{
					Pressed();
				};

			if (sprite->GetTexture() != hovered)
			{
				sprite->SetTexture(hovered);
				InputManager::EditorRegisterCallbackForMouseButtonState(KEY_PRESS, MOUSE_BUTTON_LEFT, &mousePress, "ButtonPress");
			}

			return true;
		}
		else
		{
			if (sprite->GetTexture() != base)
			{
				sprite->SetTexture(base);
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
		if (sprite->GetTexture() != pressed)
		{
			sprite->SetTexture(pressed);
			if (pressFunction != nullptr)
			{
				(*pressFunction)();
			}
		}
	}
	
	return true;
}
