#ifndef GO3DGLYPH_H
#define GO3DGLYPH_H

#include "GO3D.h"
#include "../Font/Font.h"

#include <glm/glm.hpp>

class GO3DGlyph : public GO3D
{
public:

	GO3DGlyph(const Font::Glyph& glyph, const glm::vec4& color, const glm::vec2& position, const glm::vec2& scale = glm::vec2(1.0f));

	~GO3DGlyph();

	void Update() override;

	const Font::Glyph& GetGlyph() const;

	void UpdateGlyphData(const Font::Glyph& newGlyph);

private:

	GO3DGlyph(const GO3DGlyph&) = delete;

	GO3DGlyph& operator=(const GO3DGlyph&) = delete;

	GO3DGlyph(const GO3DGlyph&&) = delete;

	GO3DGlyph& operator=(const GO3DGlyph&&) = delete;
	
	Font::Glyph glyph;

	unsigned int colorBuffer;

	glm::vec4 color;

	glm::vec2 size;

};

#endif // GO3DGLYPH_H