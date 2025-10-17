#include "TextField.h"

#include "../Renderer/Text/Text.h"
#include "../Renderer/Window/WindowManager.h"
#include "../Math/Math.h"

#include <numeric>

TextField::TextField(const std::string& t, const std::string& font, const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) :
	stretch(true)
{
	Window* window = WindowManager::GetWindow("Engine");
	float width = static_cast<float>(prevScreenWidth = window->GetWidth());
	float height = static_cast<float>(prevScreenHeight = window->GetHeight());

	float xPos = Math::ChangeRange(0.0f, 1.0f, 0.0f, width, position.x);
	float yPos = Math::ChangeRange(0.0f, 1.0f, 0.0f, height, position.y);

	int gcd = std::gcd(prevScreenWidth, prevScreenHeight);
	float simplifiedWidth = width / gcd;
	float simplifiedHeight = height / gcd;

	float xScale = Math::ChangeRange(0.0f, 1.0f, 0.0f, width, size.x * simplifiedHeight * 0.05f);
	float yScale = Math::ChangeRange(0.0f, 1.0f, 0.0f, height, size.y * simplifiedWidth * 0.05f);

	text = new Text(t, font, color, { xPos, yPos }, {xScale, yScale});

	windowResizeCallback = new std::function<void(unsigned int, unsigned int)>([this](unsigned int w, unsigned int h)
		{
			float width = static_cast<float>(w);
			float height = static_cast<float>(h);

			glm::vec2 currentPos = text->GetPosition();

			float xPos = Math::ChangeRange(0.0f, 1.0f, 0.0f, width, currentPos.x / prevScreenWidth);
			float yPos = Math::ChangeRange(0.0f, 1.0f, 0.0f, height, currentPos.y / prevScreenHeight);

			text->SetPosition({ xPos, yPos });

			glm::vec2 currentScale = text->GetScale();

			float xScale = Math::ChangeRange(0.0f, 1.0f, 0.0f, width, currentScale.x / prevScreenWidth);
			float yScale = Math::ChangeRange(0.0f, 1.0f, 0.0f, height, currentScale.y / prevScreenHeight);

			if (stretch)
			{
				text->SetScale({xScale, yScale});
			}

			prevScreenWidth = w;
			prevScreenHeight = h;
		});

	static unsigned int textFieldId = 0;

	window->RegisterCallbackForWindowResize("TextField" + std::to_string(ID = textFieldId++), windowResizeCallback);
}

TextField::~TextField()
{
	if (windowResizeCallback != nullptr)
	{
		Window* window = WindowManager::GetWindow("Engine");
		window->DeregisterCallbackForWindowResize("TextField" + std::to_string(ID));

		delete windowResizeCallback;
	}
}

void TextField::SetPosition(const glm::vec2& newPos)
{
	Window* window = WindowManager::GetWindow("Engine");

	float width = static_cast<float>(window->GetWidth());
	float height = static_cast<float>(window->GetHeight());

	float xPos = Math::ChangeRange(0.0f, 1.0f, 0.0f, width, newPos.x);
	float yPos = Math::ChangeRange(0.0f, 1.0f, 0.0f, height, newPos.y);

	text->SetPosition({ xPos, yPos });
}

glm::vec2 TextField::GetPosition() const
{
	glm::vec2 currentPositionInPixels = text->GetPosition();

	Window* window = WindowManager::GetWindow("Engine");

	float width = static_cast<float>(window->GetWidth());
	float height = static_cast<float>(window->GetHeight());

	float xPos = Math::ChangeRange(0.0f, width, 0.0f, 1.0f, currentPositionInPixels.x);
	float yPos = Math::ChangeRange(0.0f, height, 0.0f, 1.0f, currentPositionInPixels.y);

	return { xPos, yPos };
}

const std::string& TextField::GetString() const
{
	return text->GetString();
}

const std::string& TextField::GetFontName() const
{
	return text->GetFontName();
}

glm::vec3 TextField::GetColor() const
{
	return text->GetColor();
}

glm::vec2 TextField::GetScale() const
{
	return text->GetScale();
}

void TextField::SetZ(float newZ)
{
	text->SetZ(newZ);
}

void TextField::Disable()
{
	text->Disable();
}

void TextField::Enable()
{
	text->Enable();
}

bool TextField::IsDisabled() const
{
	return text->IsDisabled();
}