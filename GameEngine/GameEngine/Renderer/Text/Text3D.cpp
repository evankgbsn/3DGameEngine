#include "Text3D.h"

#include "../GraphicsObjects/GraphicsObjectManager.h"
#include "../Font/FontManager.h"
#include "../GraphicsObjects/GO3DGlyph.h"

#include <glm/gtc/matrix_transform.hpp> // Added for glm::rotate

Text3D::Text3D(const std::string& string, const std::string& fn, const glm::vec4& c, const glm::vec3& p, const glm::vec2& s) :
	text(string),
	scale(s),
	fontName(fn),
	color(c),
	disabled(false),
	position(p),
	rotation(glm::mat4(1.0f))
{
	Font* font = FontManager::GetFont(fontName);

	if (font != nullptr)
	{
		glm::vec3 rightVector = glm::normalize(glm::vec3(rotation[0]));
		glm::vec3 currentPos = position;

		for (const char& character : text)
		{
			const Font::Glyph& glyph = font->GetCharacterGlyph(character);

			GO3DGlyph* newGlyph = GraphicsObjectManager::CreateGO3DGlyph(glyph, glm::vec4(color, 1.0f), currentPos, scale);
			newGlyph->SetRotation(rotation);
			newGlyph->SetTranslation(currentPos);
			glyphs.push_back(newGlyph);

			currentPos += rightVector * ((glyph.advance >> 6) * scale.x);
		}
	}
}

void Text3D::SetPosition(const glm::vec3& newPosition)
{
	position = newPosition;

	Font* font = FontManager::GetFont(fontName);
	if (font == nullptr) return;

	glm::vec3 rightVector = glm::normalize(glm::vec3(rotation[0]));
	glm::vec3 currentPos = position;
	unsigned int index = 0;

	for (const char& character : text)
	{
		const Font::Glyph& glyph = font->GetCharacterGlyph(character);

		glyphs[index]->SetRotation(rotation);
		glyphs[index]->SetTranslation(currentPos);

		currentPos += rightVector * ((glyph.advance >> 6) * scale.x);
		index++;
	}
}

void Text3D::SetRotation(const glm::mat4& newRotation)
{
	rotation = newRotation;

	// Automatically recalculate all the letter positions along the new axis
	SetPosition(position);
}

void Text3D::SetRotation(float angle, const glm::vec3& axis)
{
	// Generates a 4x4 rotation matrix starting from an identity matrix
	rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis);

	// Snap the letters into their new places
	SetPosition(position);
}

glm::vec2 Text3D::GetPosition() const
{
	return position;
}

const std::string& Text3D::GetString() const
{
	return text;
}

const std::string& Text3D::GetFontName() const
{
	return fontName;
}

glm::vec3 Text3D::GetColor() const
{
	return color;
}

glm::vec2 Text3D::GetScale() const
{
	return scale;
}

void Text3D::SetScale(const glm::vec2& newScale)
{
	scale = newScale;

	for (GO3DGlyph* glyph : glyphs)
	{
		glyph->SetScale(glm::vec3(newScale, 1.0f));
	}

	// Re-run the positioning logic so the spacing adjusts to the new scale
	SetPosition(position);
}

void Text3D::Disable()
{
	for (GO3DGlyph* glyph : glyphs)
	{
		GraphicsObjectManager::Disable(glyph);
	}
	disabled = true;
}

void Text3D::Enable()
{
	for (GO3DGlyph* glyph : glyphs)
	{
		GraphicsObjectManager::Enable(glyph);
	}
	disabled = false;
}

bool Text3D::IsDisabled() const
{
	return disabled;
}

void Text3D::Append(const std::string& string)
{
	Font* font = FontManager::GetFont(fontName);
	if (font == nullptr) return;

	glm::vec3 rightVector = glm::normalize(glm::vec3(rotation[0]));
	glm::vec3 currentPos = position;

	// Fast-forward the position to the end of the existing text
	for (unsigned int i = 0; i < text.size(); i++)
	{
		const Font::Glyph& glyph = font->GetCharacterGlyph(text[i]);
		currentPos += rightVector * ((glyph.advance >> 6) * scale.x);
	}

	for (const char& c : string)
	{
		text.append(std::string() + c);
		const Font::Glyph& glyph = font->GetCharacterGlyph(c);

		GO3DGlyph* newGlyph = GraphicsObjectManager::CreateGO3DGlyph(glyph, glm::vec4(color, 1.0f), currentPos, scale);
		newGlyph->SetRotation(rotation);
		newGlyph->SetTranslation(currentPos);
		glyphs.push_back(newGlyph);

		currentPos += rightVector * ((glyph.advance >> 6) * scale.x);
	}
}

void Text3D::PopBack()
{
	if (glyphs.size() > 0)
	{
		GraphicsObjectManager::Delete(glyphs.back());
		glyphs.pop_back();
		text.pop_back();
	}
}

void Text3D::PopFront()
{
	if (glyphs.size() > 0)
	{
		GraphicsObjectManager::Delete(glyphs.front());
		glyphs = std::vector<GO3DGlyph*>(glyphs.begin() + 1, glyphs.end());
		text = std::string(text.begin() + 1, text.end());
	}
}

void Text3D::SetText3D(const std::string& string)
{
	if (text == string) return;

	for (GO3DGlyph* glyph : glyphs)
	{
		GraphicsObjectManager::Delete(glyph);
	}
	glyphs.clear();
	text = string;

	Font* font = FontManager::GetFont(fontName);

	if (font != nullptr)
	{
		glm::vec3 rightVector = glm::normalize(glm::vec3(rotation[0]));
		glm::vec3 currentPos = position;

		for (const char& character : text)
		{
			const Font::Glyph& glyph = font->GetCharacterGlyph(character);

			GO3DGlyph* newGlyph = GraphicsObjectManager::CreateGO3DGlyph(glyph, glm::vec4(color, 1.0f), currentPos, scale);
			newGlyph->SetRotation(rotation);
			newGlyph->SetTranslation(currentPos);
			glyphs.push_back(newGlyph);

			currentPos += rightVector * ((glyph.advance >> 6) * scale.x);
		}
	}
}

float Text3D::GetCursorPosition() const
{
	Font* font = FontManager::GetFont(fontName);

	if (font != nullptr)
	{
		float x = position.x;
		for (const char& character : text)
		{
			const Font::Glyph& glyph = font->GetCharacterGlyph(character);
			x += (glyph.advance >> 6) * scale.x;
		}

		return x;
	}

	return position.x;
}

Text3D::~Text3D()
{
	for (GO3DGlyph* glyph : glyphs)
	{
		GraphicsObjectManager::Delete(glyph);
	}
}