#include "Text.h"

#include "../GraphicsObjects/GraphicsObjectManager.h"
#include "../Font/FontManager.h"

Text::Text(const std::string& string, const std::string& fontName, const glm::vec4& color, const glm::vec2& position, float scale) :
	text(string)
{
	Font* font = FontManager::GetFont(fontName);

	if (font != nullptr)
	{
		float x = position.x;
		for (const char& character : string)
		{
			const Font::Glyph& glyph = font->GetCharacterGlyph(character);
			glyphs.push_back(GraphicsObjectManager::CreateGOGlyph(glyph, color, {x, position.y}, scale));

			x += (glyph.advance >> 6) * scale;
		}
	}
}

Text::~Text()
{
}
