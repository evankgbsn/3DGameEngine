#include "TextComponent.h"

#include "../../Renderer/Text/Text.h"

TextComponent::TextComponent()
{
	RegisterComponentClassType<TextComponent>(this);
}

TextComponent::TextComponent(const std::string& string, const std::string& fontName, const glm::vec4& color, const glm::vec2& position, const glm::vec2& scale)
{
	RegisterComponentClassType<TextComponent>(this);
	text = new Text(string, fontName, color, position, scale);
}

TextComponent::~TextComponent()
{
	delete text;
}

glm::vec2 TextComponent::GetPosition() const
{
	return text->GetPosition();
}

void TextComponent::SetPosition(const glm::vec2& newPosition)
{
	text->SetPosition(newPosition);
}

const std::string& TextComponent::GetString() const
{
	return text->GetString();
}

void TextComponent::Serialize()
{
	savedStrings["Text"] = text->GetString();
	savedStrings["FontName"] = text->GetFontName();
	savedVec2s["Position"] = text->GetPosition();
	savedVec3s["Color"] = text->GetColor();
	savedVec2s["Scale"] = text->GetScale();
}

void TextComponent::Deserialize()
{
	if (text != nullptr)
	{
		delete text;
	}

	text = new Text(savedStrings["Text"], savedStrings["FontName"], glm::vec4(savedVec3s["Color"], 1.0f), savedVec2s["Position"], savedVec2s["Scale"]);
}

void TextComponent::Update()
{
}
