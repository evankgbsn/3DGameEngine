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

	~Text();

private:

	Text() = delete;

	Text(const Text&) = delete;

	Text& operator=(const Text&) = delete;

	Text(Text&&) = delete;

	Text& operator=(Text&&) = delete;

	std::vector<GOGlyph*> glyphs;

	std::string text;

};

#endif // TEXT_H