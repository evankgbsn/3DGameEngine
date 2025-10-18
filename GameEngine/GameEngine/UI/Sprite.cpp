#include "Sprite.h"

#include "../Renderer/Texture/TextureManager.h"
#include "../Renderer/Texture/Texture.h"
#include "../Renderer/GraphicsObjects/GOSprite.h"
#include "../Renderer/Window/WindowManager.h"
#include "../Math/Math.h"
#include "../Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "../Renderer/Model/ModelManager.h"

Sprite::Sprite(const std::string& imageTexture, const glm::vec2& initialPosition, const glm::vec2& scale) :
	stretch(true)
{
	Window* window = WindowManager::GetWindow("Engine");
	float width = static_cast<float>(prevWindowWidth = window->GetWidth());
	float height = static_cast<float>(prevWindowHeight = window->GetHeight());

	float xPos = Math::ChangeRange(0.0f, 1.0f, 0.0f, width, initialPosition.x);
	float yPos = Math::ChangeRange(0.0f, 1.0f, 0.0f, height, initialPosition.y);

	float xScale = Math::ChangeRange(0.0f, 1.0f, 0.0f, width, scale.x);
	float yScale = Math::ChangeRange(0.0f, 1.0f, 0.0f, height, scale.y);

	sprite = GraphicsObjectManager::CreateGOSprite(ModelManager::GetModel("RectangleWithDepth"), TextureManager::GetTexture(imageTexture), { xPos, yPos });
	sprite->SetScale(xScale / 2, yScale / 2);

	windowResizeCallback = new std::function<void(unsigned int, unsigned int)>([this](unsigned int w, unsigned int h)
		{
			float width = static_cast<float>(w);
			float height = static_cast<float>(h);

			glm::vec2 currentPos = sprite->GetPosition();

			float xPos = Math::ChangeRange(0.0f, 1.0f, 0.0f, width, currentPos.x / prevWindowWidth);
			float yPos = Math::ChangeRange(0.0f, 1.0f, 0.0f, height, currentPos.y / prevWindowHeight);

			sprite->SetPosition({ xPos, yPos });

			glm::vec2 currentScale = sprite->GetScale();

			float xScale = Math::ChangeRange(0.0f, 1.0f, 0.0f, width, currentScale.x / prevWindowWidth);
			float yScale = Math::ChangeRange(0.0f, 1.0f, 0.0f, height, currentScale.y / prevWindowHeight);

			if (stretch)
			{
				sprite->SetScale(xScale, yScale);
			}

			prevWindowWidth = w;
			prevWindowHeight = h;
		});

	static unsigned int spriteId = 0;

	window->RegisterCallbackForWindowResize("Sprite" + std::to_string(ID = spriteId++), windowResizeCallback);
}

Sprite::~Sprite()
{
	if (windowResizeCallback != nullptr)
	{
		Window* window = WindowManager::GetWindow("Engine");
		window->DeregisterCallbackForWindowResize("Sprite" + std::to_string(ID));
		delete windowResizeCallback;
		GraphicsObjectManager::Delete(sprite);
	}
}

void Sprite::SetTexture(const std::string& textureName)
{
	sprite->SetTexture(TextureManager::GetTexture(textureName));
}

void Sprite::SetPosition(const glm::vec2& newPosition)
{
	Window* window = WindowManager::GetWindow("Engine");
	float width = static_cast<float>(prevWindowWidth = window->GetWidth());
	float height = static_cast<float>(prevWindowHeight = window->GetHeight());

	float xPos = Math::ChangeRange(0.0f, 1.0f, 0.0f, width, newPosition.x);
	float yPos = Math::ChangeRange(0.0f, 1.0f, 0.0f, height, newPosition.y);

	sprite->SetPosition({ xPos, yPos });
}

void Sprite::SetScale(float x, float y)
{
	Window* window = WindowManager::GetWindow("Engine");
	float width = static_cast<float>(prevWindowWidth = window->GetWidth());
	float height = static_cast<float>(prevWindowHeight = window->GetHeight());

	float xScale = Math::ChangeRange(0.0f, 1.0f, 0.0f, width, x);
	float yScale = Math::ChangeRange(0.0f, 1.0f, 0.0f, height, y);

	sprite->SetScale(xScale / 2, yScale / 2);
}

glm::vec2 Sprite::GetScale() const
{
	Window* window = WindowManager::GetWindow("Engine");
	float width = static_cast<float>(window->GetWidth());
	float height = static_cast<float>(window->GetHeight());

	glm::vec2 currentScale = sprite->GetScale();

	float xScale = Math::ChangeRange(0.0f, width, 0.0f, 1.0f, currentScale.x * 2);
	float yScale = Math::ChangeRange(0.0f, height, 0.0f, 1.0f, currentScale.y * 2);

	return {xScale, yScale};
}

void Sprite::Enable()
{
	GraphicsObjectManager::Enable(sprite);
}

void Sprite::Disable()
{
	GraphicsObjectManager::Disable(sprite);
}

bool Sprite::IsDisabled()
{
	return sprite->IsDisabled();
}

void Sprite::Translate(const glm::vec2& translation)
{
	Window* window = WindowManager::GetWindow("Engine");
	float width = static_cast<float>(prevWindowWidth = window->GetWidth());
	float height = static_cast<float>(prevWindowHeight = window->GetHeight());

	float xPos = Math::ChangeRange(0.0f, 1.0f, 0.0f, width, translation.x);
	float yPos = Math::ChangeRange(0.0f, 1.0f, 0.0f, height, translation.y);

	sprite->Translate({ xPos, yPos });
}

void Sprite::SetZ(float z)
{
	sprite->SetZ(z);
}

bool Sprite::Hovered() const
{
	Window* const window = WindowManager::GetWindow("Engine");
	float windowWidth = (float)window->GetWidth();
	float windowHeight = (float)window->GetHeight();

	glm::vec2 cursorPos = window->GetCursorPosition();
	cursorPos.y = windowHeight - cursorPos.y;

	if (Math::PointIn2DModel(sprite->GetModel(), glm::mat4(1.0f), sprite->GetProjection(), sprite->GetModelMat(), cursorPos, {windowWidth, windowHeight}))
	{
		return true;
	}

	return false;
}

const std::string& Sprite::GetTexture() const
{
	return sprite->GetTexture()->GetName();
}

glm::vec2 Sprite::GetPosition() const
{
	Window* window = WindowManager::GetWindow("Engine");
	float width = static_cast<float>(window->GetWidth());
	float height = static_cast<float>(window->GetHeight());

	glm::vec2 pos = sprite->GetPosition();

	float xPos = Math::ChangeRange(0.0f, width, 0.0f, 1.0f, pos.x);
	float yPos = Math::ChangeRange(0.0f, height, 0.0f, 1.0f, pos.y);

	return {xPos, yPos};
}