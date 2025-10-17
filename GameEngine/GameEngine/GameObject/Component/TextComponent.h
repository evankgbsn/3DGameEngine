#ifndef TEXTCOMPONENT_H
#define TEXTCOMPONENT_H

#include "Component.h"

#include <glm/glm.hpp>

class Text;

class TextComponent : public Component
{
public:

	TextComponent();

	TextComponent(const std::string& string, const std::string& fontName, const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f }, const glm::vec2& position = { 50.0f, 50.0f }, const glm::vec2& scale = glm::vec2(1.0f));

	~TextComponent();

	glm::vec2 GetPosition() const;

	void SetPosition(const glm::vec2& newPosition);

	const std::string& GetString() const;

private:

	TextComponent(const TextComponent&) = delete;

	TextComponent& operator=(const TextComponent&) = delete;

	TextComponent(TextComponent&&) = delete;

	TextComponent& operator=(TextComponent&&) = delete;

	void Update() override;

	void Serialize() override;

	void Deserialize() override;

	Text* text;

};


#endif //TEXTCOMPONENT_H