#include "GOGlyph.h"

#include "../Shader/ShaderManager.h"
#include "../Model/ModelManager.h"
#include "../Model/Model.h"
#include "../Window/WindowManager.h"
#include "../Window/Window.h"

#include <GL/glew.h> 

GOGlyph::GOGlyph(const Font::Glyph& g, const glm::vec4& c, const glm::vec2& pos, float scale) :
	GraphicsObject(ModelManager::LoadModel(std::string("Glyph_W") + std::to_string(g.size.x * scale) + "H" + std::to_string(g.size.y * scale),
		{
			Vertex(glm::vec3(g.bearing.x * scale, (position.y - (g.size.y - g.bearing.y) * scale) + g.size.y * scale, 0.0f), {}, {0.0f, 0.0f}),
			Vertex(glm::vec3(g.bearing.x * scale, position.y - (g.size.y - g.bearing.y) * scale, 0.0f), {}, {0.0f, 1.0f}),
			Vertex(glm::vec3((position.x + g.bearing.x * scale) + g.size.x * scale, position.y - (g.size.y - g.bearing.y) * scale, 0.0f), {}, {1.0f, 1.0f}),

			Vertex(glm::vec3(g.bearing.x * scale, ((g.size.y - g.bearing.y) * scale) + g.size.y * scale, 0.0f), {}, {0.0f, 0.0f}),
			Vertex(glm::vec3((g.bearing.x * scale) + g.size.x * scale, (g.size.y - g.bearing.y) * scale, 0.0f), {}, {1.0f, 1.0f}),
			Vertex(glm::vec3((g.bearing.x * scale) + g.size.x * scale, ((g.size.y - g.bearing.y) * scale) + g.size.y * scale, 0.0f), {}, {1.0f, 0.0f})
		}, 
		{0,1,2,3,4,5}
	)),
	glyph(g),
	color(c),
	position(pos)
{
	glCreateBuffers(1, &projectionBuffer);
	glNamedBufferStorage(projectionBuffer, sizeof(projection), &projection, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &colorBuffer);
	glNamedBufferStorage(colorBuffer, sizeof(glm::vec4), &color, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &positionBuffer);
	glNamedBufferStorage(positionBuffer, sizeof(position), &position, GL_DYNAMIC_STORAGE_BIT);
}

GOGlyph::~GOGlyph()
{
	glDeleteBuffers(1, &colorBuffer);
	glDeleteBuffers(1, &projectionBuffer);
	glDeleteBuffers(1, &positionBuffer);
}

void GOGlyph::Update()
{
	ShaderManager::StartShaderUsage("Font");

	model->BindBuffer();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, glyph.textureID);

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, projectionBuffer);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, colorBuffer);
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, positionBuffer);

	projection = glm::ortho(0.0f, (float)WindowManager::GetWindow("Engine")->GetWidth(), 0.0f, (float)WindowManager::GetWindow("Engine")->GetHeight());

	glNamedBufferSubData(projectionBuffer, 0, sizeof(projection), &projection);
	glNamedBufferSubData(colorBuffer, 0, sizeof(color), &color);
	glNamedBufferSubData(positionBuffer, 0, sizeof(position), &position);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisable(GL_BLEND);

	ShaderManager::EndShaderUsage("Font");
}
