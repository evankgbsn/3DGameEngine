#include "TextComponent.h"

#include "../../Renderer/Text/Text.h"

TextComponent::TextComponent(const std::string& string, const std::string& fontName, const glm::vec4& color, const glm::vec2& position, float scale)
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

const std::vector<char> TextComponent::Serialize() const
{
	return std::vector<char>();
}

void TextComponent::Deserialize(const std::vector<char>& data)
{
}

void TextComponent::Update()
{
}
