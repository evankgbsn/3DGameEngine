#ifndef TEXT_H
#define TEXT_H

#include <string>
#include <vector>

#include <glm/glm.hpp>

class GOGlyph;

class Text
{

public:

	Text(const std::string& string, const std::string& fontName, const glm::vec4& color = {1.0f, 1.0f, 1.0f, 1.0f}, const glm::vec2& position = { 50.0f, 50.0f }, float scale = 1.0f);

	void SetPosition(const glm::vec2& newPosition);

	glm::vec2 GetPosition() const;

	const std::string& GetString() const;

	const std::string& GetFontName() const;

	glm::vec3 GetColor() const;

	float GetScale() const;

	void SetZ(float newZ);

	void Disable();

	void Enable();

	bool IsDisabled() const;

	~Text();

private:

	Text() = delete;

	Text(const Text&) = delete;

	Text& operator=(const Text&) = delete;

	Text(Text&&) = delete;

	Text& operator=(Text&&) = delete;

	std::vector<GOGlyph*> glyphs;

	std::string text;

	float scale;

	glm::vec3 color;

	glm::vec2 position;

	std::string fontName;

	bool disabled;

};

#endif // TEXT_H