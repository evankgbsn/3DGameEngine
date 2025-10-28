#include "GOColoredInstanced.h"

#include "../Model/Model.h"
#include "../Camera/CameraManager.h"
#include "../Camera/Camera.h"
#include "../Shader/ShaderManager.h"

#include <gl/glew.h>

std::list<unsigned int> GOColoredInstanced::removedInstances;

GOColoredInstanced::GOColoredInstanced(Model* const model, const glm::vec4& initialColor, unsigned int instanceCount) :
	GraphicsObject(model),
	translations(std::vector<glm::mat4>(instanceCount)),
	rotations(std::vector<glm::mat4>(instanceCount)),
	scales(std::vector<glm::mat4>(instanceCount)),
	transformations(std::vector<glm::mat4>(instanceCount)),
	transforms(std::vector<glm::mat4>(instanceCount)),
	colors(std::vector<glm::vec4>(instanceCount)),
	rights(std::vector<glm::vec4>(instanceCount)),
	ups(std::vector<glm::vec4>(instanceCount)),
	forwards(std::vector<glm::vec4>(instanceCount)),
	trans(std::vector<glm::vec4>(instanceCount)),
	clipPlane(ShaderManager::GetClipPlane())
{
	glCreateBuffers(1, &viewProjectionBuffer);
	glNamedBufferStorage(viewProjectionBuffer, sizeof(vp), &vp, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &clipPlaneBuffer);
	glNamedBufferStorage(clipPlaneBuffer, sizeof(clipPlane), &clipPlane, GL_DYNAMIC_STORAGE_BIT);

	glGenBuffers(1, &translationsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, translationsBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * transforms.size(), &transforms[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &rightBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, rightBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * transforms.size(), &transforms[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &upBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, upBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * transforms.size(), &transforms[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &forwardBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, forwardBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * transforms.size(), &transforms[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	colors[0] = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	glGenBuffers(1, &colorsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorsBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * colors.size(), &colors[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glEnableVertexAttribArray(5);
	glBindBuffer(GL_ARRAY_BUFFER, translationsBuffer);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribDivisor(5, 1);

	glEnableVertexAttribArray(6);
	glBindBuffer(GL_ARRAY_BUFFER, rightBuffer);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribDivisor(6, 1);

	glEnableVertexAttribArray(7);
	glBindBuffer(GL_ARRAY_BUFFER, upBuffer);
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribDivisor(7, 1);

	glEnableVertexAttribArray(8);
	glBindBuffer(GL_ARRAY_BUFFER, forwardBuffer);
	glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribDivisor(8, 1);

	glEnableVertexAttribArray(9);
	glBindBuffer(GL_ARRAY_BUFFER, colorsBuffer);
	glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribDivisor(9, 1);

	FinalizeTransforms();

}

GOColoredInstanced::~GOColoredInstanced()
{
	glDeleteBuffers(1, &viewProjectionBuffer);
	glDeleteBuffers(1, &clipPlaneBuffer);
	glDeleteBuffers(1, &translationsBuffer);
	glDeleteBuffers(1, &rightBuffer);
	glDeleteBuffers(1, &upBuffer);
	glDeleteBuffers(1, &forwardBuffer);
	glDeleteBuffers(1, &colorsBuffer);
}

void GOColoredInstanced::SetColor(const glm::vec4& color, unsigned int instanceID)
{
	colors[instanceID] = color;

	glBindBuffer(GL_ARRAY_BUFFER, colorsBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * colors.size(), colors.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

const glm::vec4& GOColoredInstanced::GetColor(unsigned int instanceID) const
{
	return colors[instanceID];
}

void GOColoredInstanced::Update()
{
	ShaderManager::StartShaderUsage("ColoredInstanced");

	model->BindBuffer();

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, viewProjectionBuffer);
	glBindBufferBase(GL_UNIFORM_BUFFER, 10, clipPlaneBuffer);

	const Camera& cam = CameraManager::GetActiveCamera();
	vp.view = cam.GetView();
	vp.projection = cam.GetProjection();

	clipPlane.plane = ShaderManager::GetClipPlane();

	glNamedBufferSubData(viewProjectionBuffer, 0, sizeof(ViewProjectionUBO), &vp);
	glNamedBufferSubData(clipPlaneBuffer, 0, sizeof(ClipPlaneUBO), &clipPlane);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glDrawElements(GL_TRIANGLES, (int)model->GetIndices().size(), GL_UNSIGNED_INT, 0);
	glDrawElementsInstanced(GL_TRIANGLES, (int)model->GetIndices().size(), GL_UNSIGNED_INT, 0, (unsigned int)transforms.size());

	ShaderManager::EndShaderUsage("ColoredInstanced");
}

unsigned int GOColoredInstanced::AddInstance()
{
	if (removedInstances.empty())
	{
		translations.push_back(glm::mat4(1.0f));
		rotations.push_back(glm::mat4(1.0f));
		scales.push_back(glm::mat4(1.0f));
		transformations.push_back(glm::mat4(1.0f));
		transforms.push_back(glm::mat4(1.0f));
		colors.push_back(glm::vec4(1.0f));

		trans.push_back(glm::vec4(1.0f));
		rights.push_back(glm::vec4(1.0f));
		ups.push_back(glm::vec4(1.0f));
		forwards.push_back(glm::vec4(1.0f));

		return translations.size() - 1;
	}
	
	unsigned int newInstanceID = removedInstances.front();
	removedInstances.pop_front();
	
	translations[newInstanceID] = (glm::mat4(1.0f));
	rotations[newInstanceID] = (glm::mat4(1.0f));
	scales[newInstanceID] = (glm::mat4(1.0f));
	transformations[newInstanceID] = (glm::mat4(1.0f));
	transforms[newInstanceID] = (glm::mat4(1.0f));
	colors[newInstanceID] = (glm::vec4(1.0f));

	trans[newInstanceID] = (glm::vec4(1.0f));
	rights[newInstanceID] = (glm::vec4(1.0f));
	ups[newInstanceID] = (glm::vec4(1.0f));
	forwards[newInstanceID] = (glm::vec4(1.0f));

	return newInstanceID;
}

void GOColoredInstanced::RemoveInstanceByID(unsigned int instanceID)
{
	translations[instanceID] = glm::mat4(0.0f);
	rotations[instanceID] = glm::mat4(0.0f);
	scales[instanceID] = glm::mat4(0.0f);
	transformations[instanceID] = glm::mat4(0.0f);
	transforms[instanceID] = glm::mat4(0.0f);
	colors[instanceID] = glm::vec4(0.0f);
	trans[instanceID] = glm::vec4(0.0f);
	rights[instanceID] = glm::vec4(0.0f);
	ups[instanceID] = glm::vec4(0.0f);
	forwards[instanceID] = glm::vec4(0.0f);

	removedInstances.push_back(instanceID);
}

unsigned int GOColoredInstanced::GetInstanceCount()
{
	return static_cast<unsigned int>(transforms.size());
}

void GOColoredInstanced::FinalizeTransforms()
{
	for (unsigned int i = 0; i < transformations.size(); ++i)
	{
		transforms[i] = translations[i] * rotations[i] * scales[i];
		rights[i] = transforms[i][0];
		ups[i] = transforms[i][1];
		forwards[i] = transforms[i][2];
		trans[i] = transforms[i][3];
	}

	glBindBuffer(GL_ARRAY_BUFFER, translationsBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * trans.size(), trans.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, rightBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * rights.size(), rights.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, upBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * ups.size(), ups.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, forwardBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * forwards.size(), forwards.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, colorsBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * colors.size(), colors.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GOColoredInstanced::UpdateInstanceByID(unsigned int instanceID)
{
	transforms[instanceID] = translations[instanceID] * rotations[instanceID] * scales[instanceID];
	rights[instanceID] = transforms[instanceID][0];
	ups[instanceID] = transforms[instanceID][1];
	forwards[instanceID] = transforms[instanceID][2];
	trans[instanceID] = transforms[instanceID][3];

	glNamedBufferSubData(translationsBuffer, instanceID * sizeof(glm::vec4), sizeof(glm::vec4), &trans[instanceID]);
	glNamedBufferSubData(rightBuffer, instanceID * sizeof(glm::vec4), sizeof(glm::vec4), &rights[instanceID]);
	glNamedBufferSubData(upBuffer, instanceID * sizeof(glm::vec4), sizeof(glm::vec4), &ups[instanceID]);
	glNamedBufferSubData(forwardBuffer, instanceID * sizeof(glm::vec4), sizeof(glm::vec4), &forwards[instanceID]);
	glNamedBufferSubData(colorsBuffer, instanceID * sizeof(glm::vec4), sizeof(glm::vec4), &colors[instanceID]);
}
