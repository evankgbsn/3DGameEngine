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

	void SetPosition(const glm::vec2& newPosition);

	glm::vec2 GetPosition() const;

	void SetZ(float newZ);

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

	unsigned int zBuffer;

	glm::mat4 projection;

	glm::vec2 position;

	float z;

};

#endif // GOGLYPH_H