#include "GO3DGlyph.h"

#include "../Shader/ShaderManager.h"
#include "../Model/ModelManager.h"
#include "../Model/Model.h"
#include "../Window/WindowManager.h"
#include "../Window/Window.h"

#include <GL/glew.h> 

GO3DGlyph::GO3DGlyph(const Font::Glyph& g, const glm::vec4& c, const glm::vec2& pos, const glm::vec2& s) :
	GO3D(ModelManager::LoadModel(std::string("Glyph_W") + std::to_string(g.size.x * s.x) + "H" + std::to_string(g.size.y * s.y),
		{
			Vertex(glm::vec3(g.bearing.x * s.x, (0.0f - (g.size.y - g.bearing.y) * s.y) + g.size.y * s.y, 0.0f), {}, {0.0f, 0.0f}),
			Vertex(glm::vec3(g.bearing.x * s.x, 0.0f - (g.size.y - g.bearing.y) * s.y, 0.0f), {}, {0.0f, 1.0f}),
			Vertex(glm::vec3((0.0f + g.bearing.x * s.x) + g.size.x * s.x, 0.0f - (g.size.y - g.bearing.y) * s.y, 0.0f), {}, {1.0f, 1.0f}),

			Vertex(glm::vec3(g.bearing.x * s.x, (0.0f - (g.size.y - g.bearing.y) * s.y) + g.size.y * s.y, 0.0f), {}, {0.0f, 0.0f}),
			Vertex(glm::vec3((g.bearing.x * s.x) + g.size.x * s.x, 0.0f - (g.size.y - g.bearing.y) * s.y, 0.0f), {}, {1.0f, 1.0f}),
			Vertex(glm::vec3((g.bearing.x * s.x) + g.size.x * s.x, (0.0f - (g.size.y - g.bearing.y) * s.y) + g.size.y * s.y, 0.0f), {}, {1.0f, 0.0f})
		},
		{ 0,1,2,3,4,5 },
		false
	), "Font3D"),
	glyph(g),
	color(c)
{
	glCreateBuffers(1, &colorBuffer);
	glNamedBufferStorage(colorBuffer, sizeof(glm::vec4), &color, GL_DYNAMIC_STORAGE_BIT);
}

GO3DGlyph::~GO3DGlyph()
{
	glDeleteBuffers(1, &colorBuffer);
}

void GO3DGlyph::Update()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, glyph.textureID);

	glBindBufferBase(GL_UNIFORM_BUFFER, 1, colorBuffer);
	glNamedBufferSubData(colorBuffer, 0, sizeof(color), &color);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	GO3D::Update();
	glDisable(GL_BLEND);
}

const Font::Glyph& GO3DGlyph::GetGlyph() const
{
	return glyph;
}
