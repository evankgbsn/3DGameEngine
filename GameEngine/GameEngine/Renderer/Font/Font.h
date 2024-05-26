#ifndef FONT_H
#define FONT_H

#include <unordered_map>
#include <string>

#include <glm/glm.hpp>

class Font
{
public:

	Font(const std::string& path);

	~Font();

	struct Glyph
	{
		unsigned int textureID; // ID handle of the glyph texture
		glm::ivec2 size;		// Size of glyph
		glm::ivec2 bearing;		// Offset from baseline to left/top of glyph
		unsigned int advance;	// Offset to advance to next glyph
	};

	const Glyph& GetCharacterGlyph(const char& character) const;

private:

	Font() = delete;

	Font(const Font&) = delete;

	Font& operator=(const Font&) = delete;

	Font(Font&&) = delete;

	Font& operator=(Font&&) = delete;

	std::unordered_map<char, Glyph> characters;

};

#endif // FONT_H