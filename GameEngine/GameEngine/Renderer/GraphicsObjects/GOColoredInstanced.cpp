#include "GOColoredInstanced.h"

#include "../Model/Model.h"
#include "../Camera/CameraManager.h"
#include "../Camera/Camera.h"
#include "../Shader/ShaderManager.h"

#include <gl/glew.h>

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
	trans(std::vector<glm::vec4>(instanceCount))
{
	glCreateBuffers(1, &viewProjectionBuffer);
	glNamedBufferStorage(viewProjectionBuffer, sizeof(vp), &vp, GL_DYNAMIC_STORAGE_BIT);

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

	FinalizeTransforms();

}

GOColoredInstanced::~GOColoredInstanced()
{
	glDeleteBuffers(1, &viewProjectionBuffer);
}

void GOColoredInstanced::SetColor(const glm::vec4& color, unsigned int instanceID)
{
	colors[instanceID] = color;
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

	const Camera& cam = CameraManager::GetActiveCamera();
	vp.view = cam.GetView();
	vp.projection = cam.GetProjection();

	glNamedBufferSubData(viewProjectionBuffer, 0, sizeof(ViewProjectionUBO), &vp);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glDrawElements(GL_TRIANGLES, (int)model->GetIndices().size(), GL_UNSIGNED_INT, 0);
	glDrawElementsInstanced(GL_TRIANGLES, (int)model->GetIndices().size(), GL_UNSIGNED_INT, 0, (unsigned int)transforms.size());

	ShaderManager::EndShaderUsage("ColoredInstanced");
}

void GOColoredInstanced::AddInstance()
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
}

void GOColoredInstanced::RemoveInstanceByID(unsigned int instanceID)
{
	translations.erase(translations.begin() + instanceID);
	rotations.erase(rotations.begin() + instanceID);
	scales.erase(scales.begin() + instanceID);
	transformations.erase(transformations.begin() + instanceID);
	transforms.erase(transforms.begin() + instanceID);
	colors.erase(colors.begin() + instanceID);
	trans.erase(trans.begin() + instanceID);
	rights.erase(rights.begin() + instanceID);
	ups.erase(ups.begin() + instanceID);
	forwards.erase(forwards.begin() + instanceID);
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
}
