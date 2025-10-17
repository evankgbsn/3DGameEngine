#ifndef GOGLYPH_H
#define GOGLYPH_H

#include "GraphicsObject.h"
#include "../Font/Font.h"

#include <glm/glm.hpp>

class GOGlyph : public GraphicsObject
{
public:

	GOGlyph(const Font::Glyph& glyph, const glm::vec4& color, const glm::vec2& position, const glm::vec2& scale = glm::vec2(1.0f));

	~GOGlyph();

	void Update() override;

	void SetPosition(const glm::vec2& newPosition);

	void SetScale(const glm::vec2& newScale);

	glm::vec2 GetPosition() const;

	void SetZ(float newZ);

	const Font::Glyph& GetGlyph() const;

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

	glm::vec2 scale;

	float z;

};

#endif // GOGLYPH_H