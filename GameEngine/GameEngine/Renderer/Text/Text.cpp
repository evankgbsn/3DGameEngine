#include "Text.h"

#include "../GraphicsObjects/GraphicsObjectManager.h"
#include "../Font/FontManager.h"
#include "../GraphicsObjects/GOGlyph.h"

Text::Text(const std::string& string, const std::string& fn, const glm::vec4& color, const glm::vec2& position, float s) :
	text(string),
	scale(s),
	fontName(fn)
{
	Font* font = FontManager::GetFont(fontName);

	if (font != nullptr)
	{
		float x = position.x;
		for (const char& character : text)
		{
			const Font::Glyph& glyph = font->GetCharacterGlyph(character);
			glyphs.push_back(GraphicsObjectManager::CreateGOGlyph(glyph, color, {x, position.y}, scale));

			x += (glyph.advance >> 6) * scale;
		}
	}
}

void Text::SetPosition(const glm::vec2& newPosition)
{
	Font* font = FontManager::GetFont(fontName);

	float x = newPosition.x;
	unsigned int index = 0;
	for (const char& character : text)
	{
		const Font::Glyph& glyph = font->GetCharacterGlyph(character);
		glyphs[index++]->SetPosition({ x, newPosition.y });
		x += (glyph.advance >> 6) * scale;
	}
}

glm::vec2 Text::GetPosition() const
{
	return glyphs.front()->GetPosition();
}

Text::~Text()
{
	for (GOGlyph* glyph : glyphs)
	{
		GraphicsObjectManager::Delete(glyph);
	}
}
