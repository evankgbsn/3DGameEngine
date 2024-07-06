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
	colors(std::vector<glm::vec4>(instanceCount))
{
	glCreateBuffers(1, &viewProjectionBuffer);
	glNamedBufferStorage(viewProjectionBuffer, sizeof(vp), &vp, GL_DYNAMIC_STORAGE_BIT);

	glGenBuffers(1, &translationsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, translationsBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * transforms.size(), &transforms[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &rightBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, rightBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * transforms.size(), &transforms[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &upBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, upBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * transforms.size(), &transforms[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &forwardBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, forwardBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * transforms.size(), &transforms[0], GL_STATIC_DRAW);
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

	for (unsigned int i = 0; i < transformations.size(); ++i)
	{
		transforms[i] = translations[i] * rotations[i] * scales[i];
	}

	std::vector<glm::vec4> translations;
	std::vector<glm::vec4> rights;
	std::vector<glm::vec4> ups;
	std::vector<glm::vec4> forwards;

	for (glm::mat4& transform : transforms)
	{
		rights.push_back(transform[0]);
		ups.push_back(transform[1]);
		forwards.push_back(transform[2]);
		translations.push_back(transform[3]);
	}

	glBindBuffer(GL_ARRAY_BUFFER, translationsBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * translations.size(), translations.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, rightBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * rights.size(), rights.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, upBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * ups.size(), ups.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, forwardBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * forwards.size(), forwards.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

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
}

void GOColoredInstanced::RemoveInstance(unsigned int instanceID)
{
}

unsigned int GOColoredInstanced::GetInstanceCount()
{
	return static_cast<unsigned int>(transforms.size());
}
