#include "GO3D.h"

#include "../Camera/CameraManager.h"
#include "../Camera/Camera.h"
#include "../Model/Model.h"
#include "../Shader/ShaderManager.h"
#include "../Renderer/Renderer.h"
#include "GOWater.h"
#include "GOTerrain.h"

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

	glCreateBuffers(1, &clipPlaneBuffer);
	glNamedBufferStorage(clipPlaneBuffer, sizeof(glm::vec4), &clipPlane, GL_DYNAMIC_STORAGE_BIT);
}

GO3D::~GO3D()
{
	glDeleteBuffers(1, &mvpBuffer);
	glDeleteBuffers(1, &clipPlaneBuffer);
}

void GO3D::Update()
{
	model->BindBuffer();

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, mvpBuffer);
	glBindBufferBase(GL_UNIFORM_BUFFER, 10, clipPlaneBuffer);

	const Camera& cam = CameraManager::GetActiveCamera();
	mvp.view = cam.GetView();
	mvp.projection = cam.GetProjection();
	mvp.model = translation * rotation * scale;

	clipPlane = ShaderManager::GetClipPlane();

	glNamedBufferSubData(mvpBuffer, 0, sizeof(MVP), &mvp);
	glNamedBufferSubData(clipPlaneBuffer, 0, sizeof(glm::vec4), &clipPlane);

	glLineWidth(lineWidth);
	glPointSize(pointSize);

	glPolygonMode(GL_FRONT_AND_BACK, (GLenum)drawMode);
	
	if (Renderer::ShouldDraw())
	{
		GOWater* water = dynamic_cast<GOWater*>(this);
		GOTerrain* terrain = dynamic_cast<GOTerrain*>(this);
		

		if (glm::dot(cam.GetForwardVector(), glm::normalize(cam.GetPosition() - glm::vec3(translation[3]))) < 0)
		{
			glDrawElements(GL_TRIANGLES, (int)model->GetIndices().size(), GL_UNSIGNED_INT, 0);
		}
		else if (water != nullptr || terrain != nullptr)
		{
			glDrawElements(GL_TRIANGLES, (int)model->GetIndices().size(), GL_UNSIGNED_INT, 0);
		}
		else
		{
			glDrawElements(GL_TRIANGLES, (int)model->GetIndices().size(), GL_UNSIGNED_INT, 0);
		}

	}
}
