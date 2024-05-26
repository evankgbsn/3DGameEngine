#ifndef GOGLYPH_H
#define GOGLYPH_H

#include "GraphicsObject.h"
#include "../Font/Font.h"

#include <glm/glm.hpp>

class GOGlyph : public GraphicsObject
{
public:

	GOGlyph(const Font::Glyph& glyph, const glm::vec4& color, const glm::vec2& position, float scale = 1.0f);

	~GOGlyph();

	void Update() override;

private:

	GOGlyph(const GOGlyph&) = delete;

	GOGlyph& operator=(const GOGlyph&) = delete;

	GOGlyph(const GOGlyph&&) = delete;

	GOGlyph& operator=(const GOGlyph&&) = delete;
	
	Font::Glyph glyph;

	glm::vec4 color;

	unsigned int colorBuffer;

	unsigned int projectionBuffer;

	unsigned int positionBuffer;

	glm::mat4 projection;

	glm::vec2 position;

};

#endif // GOGLYPH_H