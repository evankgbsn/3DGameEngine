#include "Font.h"

#include "FontManager.h"
#include "../../Utils/Logger.h"

#include <GL/glew.h>

Font::Font(const std::string& path)
{
	FT_Face face;
	if (FT_New_Face(FontManager::instance->ft, path.c_str(), 0, &face))
	{
		Logger::Log("Failed to load font from path: " + path, Logger::Category::Error);
	}
	else
	{
		FT_Set_Pixel_Sizes(face, 0, 48);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction.

		for (unsigned char c = 0; c < 128; ++c)
		{
			// Load character glyph
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				Logger::Log("Failed to load glyph: " + c + std::string(" from font: ") + path, Logger::Category::Error);
				continue;
			}

			// Generate texture
			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

			// set texture options
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// now store character for later use
			Glyph character;
			character.textureID = texture;
			character.size = glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
			character.bearing = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
			character.advance = face->glyph->advance.x;

			characters.insert(std::pair<char, Glyph>(c, character));
		}

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		FT_Done_Face(face);

		Logger::Log("Loaded font: " + path);
	}
}

Font::~Font()
{
}

const Font::Glyph& Font::GetCharacterGlyph(const char& character) const
{
	return characters.find(character)->second;
}
