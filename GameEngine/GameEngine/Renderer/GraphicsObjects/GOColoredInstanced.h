#ifndef GOCOLOREDINSTANCED_H
#define GOCOLOREDINSTANCED_H

#include "GraphicsObject.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

class Model;

class GOColoredInstanced : public GraphicsObject
{
public:

	virtual void Translate(const glm::vec3& t, unsigned int instanceID)
	{
		translations[instanceID] = glm::translate(translations[instanceID], t);
	};

	virtual void Rotate(float angle, const glm::vec3& axis, unsigned int instanceID)
	{
		float angleInRadians = angle / 57.2957795f;
		rotations[instanceID] = glm::rotate(rotations[instanceID], angleInRadians, axis);
	};

	virtual void Scale(const glm::vec3& s, unsigned int instanceID)
	{
		scales[instanceID] = glm::scale(scales[instanceID], s);
	};

	virtual void SetTranslation(const glm::vec3& t, unsigned int instanceID)
	{
		translations[instanceID] = glm::mat4(1.0f);
		translations[instanceID] = glm::translate(translations[instanceID], t);
	};

	virtual void SetRotation(const glm::mat4& newRotation, unsigned int instanceID)
	{
		rotations[instanceID] = newRotation;
	};

	virtual void SetScale(const glm::vec3& s, unsigned int instanceID)
	{
		scales[instanceID] = glm::mat4(1.0f);
		scales[instanceID] = glm::scale(scales[instanceID], s);
	};

	virtual glm::vec3 GetTranslation(unsigned int instanceID) const
	{
		return translations[instanceID][3];
	};

	virtual glm::vec3 GetScale(unsigned int instanceID) const
	{
		return glm::vec3(scales[instanceID][0][0], scales[instanceID][1][1], scales[instanceID][2][2]);
	};

	virtual glm::mat4 GetRotation(unsigned int instanceID) const
	{
		return rotations[instanceID];
	};

	virtual void SetTransform(const glm::mat4& newTransform, unsigned int instanceID)
	{
		translations[instanceID] = glm::translate(glm::mat4(1.0f), glm::vec3(newTransform[3]));

		float scaleX = glm::length(newTransform[0]);
		float scaleY = glm::length(newTransform[1]);
		float scaleZ = glm::length(newTransform[2]);

		scales[instanceID][0] = glm::vec4(scaleX, 0.0f, 0.0f, 0.0f);
		scales[instanceID][1] = glm::vec4(0.0f, scaleY, 0.0f, 0.0f);
		scales[instanceID][2] = glm::vec4(0.0f, 0.0f, scaleZ, 0.0f);

		rotations[instanceID] = glm::mat4(1.0f);

		rotations[instanceID][0] = glm::normalize(newTransform[0]);
		rotations[instanceID][1] = glm::normalize(newTransform[1]);
		rotations[instanceID][2] = glm::normalize(newTransform[2]);
	};

	virtual const glm::mat4& GetTransform(unsigned int instanceID)
	{
		return transformations[instanceID] = translations[instanceID] * rotations[instanceID] * scales[instanceID];
	};

	GOColoredInstanced(Model* const model, const glm::vec4& initialColor, unsigned int instanceCount);

	~GOColoredInstanced();

	void SetColor(const glm::vec4& color, unsigned int instanceID);

	const glm::vec4& GetColor(unsigned int instanceID) const;

	void Update() override;

	void AddInstance();

	void RemoveInstance(unsigned int instanceID);

	unsigned int GetInstanceCount();

	void FinalizeTransforms();

	void UpdateInstanceByID(unsigned int instanceID);

private:

	struct ModelUBO
	{
		glm::mat4 model;
	};

	struct ViewProjectionUBO
	{
		glm::mat4 view;
		glm::mat4 projection;
	} vp;

	GOColoredInstanced() = delete;

	GOColoredInstanced(const GOColoredInstanced&) = delete;

	GOColoredInstanced& operator=(const GOColoredInstanced&) = delete;

	GOColoredInstanced(GOColoredInstanced&&) = delete;

	GOColoredInstanced& operator=(GOColoredInstanced&&) = delete;

	unsigned int viewProjectionBuffer;

	unsigned int colorsBuffer;

	unsigned int colorBuffer;

	unsigned int translationsBuffer;

	unsigned int rightBuffer;

	unsigned int upBuffer;

	unsigned int forwardBuffer;

	std::vector<glm::mat4> translations;

	std::vector<glm::mat4> rotations;

	std::vector<glm::mat4> scales;

	std::vector<glm::mat4> transformations;

	std::vector<glm::vec4> colors;

	std::vector<glm::mat4> transforms;

	std::vector<glm::vec4> trans;
	std::vector<glm::vec4> rights;
	std::vector<glm::vec4> ups;
	std::vector<glm::vec4> forwards;
};

#endif // GOCOLOREDINSTANCED_H