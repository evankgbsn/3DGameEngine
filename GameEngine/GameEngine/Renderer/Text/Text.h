#ifndef TEXT_H
#define TEXT_H

#include <string>
#include <vector>

#include <glm/glm.hpp>

class GOGlyph;

class Text
{

public:

	Text(const std::string& string, const std::string& fontName, const glm::vec4& color = {1.0f, 1.0f, 1.0f, 1.0f}, const glm::vec2& position = { 50.0f, 50.0f }, const glm::vec2& scale = glm::vec2(1.0f));

	void SetPosition(const glm::vec2& newPosition);

	glm::vec2 GetPosition() const;

	const std::string& GetString() const;

	const std::string& GetFontName() const;

	glm::vec3 GetColor() const;

	glm::vec2 GetScale() const;

	void SetScale(const glm::vec2& newScale);

	void SetZ(float newZ);

	void Disable();

	void Enable();

	bool IsDisabled() const;

	void Append(const std::string& string);

	void PopBack();

	void PopFront();

	void SetText(const std::string& string);

	~Text();

private:

	Text() = delete;

	Text(const Text&) = delete;

	Text& operator=(const Text&) = delete;

	Text(Text&&) = delete;

	Text& operator=(Text&&) = delete;

	std::vector<GOGlyph*> glyphs;

	std::string text;

	glm::vec2 scale;

	glm::vec3 color;

	glm::vec2 position;

	std::string fontName;

	bool disabled;

	float z;

};

#endif // TEXT_H