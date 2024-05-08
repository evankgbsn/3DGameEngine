#include "GOLineColored.h"

#include "../Model/ModelManager.h"
#include "../Model/Model.h"
#include "../Model/Vertex.h"
#include "../Camera/CameraManager.h"
#include "../Shader/ShaderManager.h"

#include <gl/glew.h>

unsigned int GOLineColored::lineModelId = 0;

GOLineColored::GOLineColored(const glm::vec3& s, const glm::vec3& e, const glm::vec4& c) :
	GraphicsObject(ModelManager::LoadModel(std::string("GOLineColored") + std::to_string(lineModelId++), { Vertex(s, {}, {}), Vertex(e, {}, {}) }, {0, 1})),
	start(s),
	end(s),
	color(c),
	mvp(),
	mvpBuffer(0),
	colorBuffer(0),
	lineWidth(3.0f)
{
	glCreateBuffers(1, &mvpBuffer);
	glNamedBufferStorage(mvpBuffer, sizeof(mvp), &mvp, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &colorBuffer);
	glNamedBufferStorage(colorBuffer, sizeof(color), &color, GL_DYNAMIC_STORAGE_BIT);
}

GOLineColored::~GOLineColored()
{
	glDeleteBuffers(1, &mvpBuffer);
	glDeleteBuffers(1, &colorBuffer);
}

void GOLineColored::Update()
{
	ShaderManager::StartShaderUsage("Colored");

	GLfloat currentLineWidth;
	glGetFloatv(GL_LINE_WIDTH, &currentLineWidth);
	glLineWidth(lineWidth);

	model->BindBuffer();

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, mvpBuffer);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, colorBuffer);

	const Camera& cam = CameraManager::GetActiveCamera();
	mvp.view = cam.GetView();
	mvp.projection = cam.GetProjection();
	mvp.model = glm::mat4(1.0f);

	glNamedBufferSubData(mvpBuffer, 0, sizeof(MVP), &mvp);
	glNamedBufferSubData(colorBuffer, 0, sizeof(color), &color);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_LINE_STRIP, (int)model->GetIndices().size(), GL_UNSIGNED_INT, 0);
	glLineWidth(currentLineWidth);

	ShaderManager::EndShaderUsage("Colored");
}
