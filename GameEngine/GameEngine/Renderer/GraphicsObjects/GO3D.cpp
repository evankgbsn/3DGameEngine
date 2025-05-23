#include "GO3D.h"

#include "../Camera/CameraManager.h"
#include "../Camera/Camera.h"
#include "../Model/Model.h"

void GO3D::SetDrawMode(Mode m)
{
	drawMode = m;
}

void GO3D::SetLineWidth(float width)
{
	lineWidth = width;
}

void GO3D::SetPointSize(float size)
{
	pointSize = size;
}

GO3D::GO3D(Model* const model) :
	GraphicsObject(model),
	mvp({ glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f) }),
	mvpBuffer(),
	translation(glm::mat4(1.0f)),
	rotation(glm::mat4(1.0f)),
	scale(glm::mat4(1.0f)),
	transformation(glm::mat4(1.0f)),
	drawMode(Mode::FILL),
	lineWidth(1.0f),
	pointSize(1.0f)
{
	glCreateBuffers(1, &mvpBuffer);
	glNamedBufferStorage(mvpBuffer, sizeof(mvp), &mvp, GL_DYNAMIC_STORAGE_BIT);
}

GO3D::~GO3D()
{
	glDeleteBuffers(1, &mvpBuffer);
}

void GO3D::Update()
{
	model->BindBuffer();

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, mvpBuffer);

	const Camera& cam = CameraManager::GetActiveCamera();
	mvp.view = cam.GetView();
	mvp.projection = cam.GetProjection();
	mvp.model = translation * rotation * scale;

	glNamedBufferSubData(mvpBuffer, 0, sizeof(MVP), &mvp);

	glLineWidth(lineWidth);
	glPointSize(pointSize);

	glPolygonMode(GL_FRONT_AND_BACK, (GLenum)drawMode);
	glDrawElements(GL_TRIANGLES, (int)model->GetIndices().size(), GL_UNSIGNED_INT, 0);
}
