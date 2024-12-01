#ifndef GO3D_H
#define GO3D_H

#include "GraphicsObject.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>

class Model;

class GO3D : public GraphicsObject
{

public:

	virtual void Translate(const glm::vec3& t)
	{
		translation = glm::translate(translation, t);
	};

	virtual void Rotate(float angle, const glm::vec3& axis)
	{
		float angleInRadians = angle / 57.2957795f;
		rotation = glm::rotate(rotation, angleInRadians, axis);
	};

	virtual void Scale(const glm::vec3& s)
	{
		scale = glm::scale(scale, s);
	};

	virtual void SetTranslation(const glm::vec3& t)
	{
		translation = glm::mat4(1.0f);
		translation = glm::translate(translation, t);
	};

	virtual void SetRotation(const glm::mat4& newRotation)
	{
		rotation = newRotation;
	};

	virtual void SetScale(const glm::vec3& s)
	{
		scale = glm::mat4(1.0f);
		scale = glm::scale(scale, s);
	};

	virtual glm::vec3 GetTranslation() const
	{
		return translation[3];
	};

	virtual glm::vec3 GetScale() const
	{
		return glm::vec3(scale[0][0], scale[1][1], scale[2][2]);
	};

	virtual glm::mat4 GetRotation() const
	{
		return rotation;
	};

	virtual void SetTransform(const glm::mat4& newTransform)
	{
		translation = glm::translate(glm::mat4(1.0f), glm::vec3(newTransform[3]));
		
		float scaleX = glm::length(newTransform[0]);
		float scaleY = glm::length(newTransform[1]);
		float scaleZ = glm::length(newTransform[2]);
		
		scale[0] = glm::vec4(scaleX, 0.0f, 0.0f, 0.0f);
		scale[1] = glm::vec4(0.0f, scaleY, 0.0f, 0.0f);
		scale[2] = glm::vec4(0.0f, 0.0f, scaleZ, 0.0f);
		
		rotation = glm::mat4(1.0f);
		
		rotation[0] = glm::normalize(newTransform[0]);
		rotation[1] = glm::normalize(newTransform[1]);
		rotation[2] = glm::normalize(newTransform[2]);
	};

	virtual const glm::mat4& GetTransform()
	{
		return transformation = translation * rotation * scale;
	};

	enum class Mode
	{
		FILL = GL_FILL,
		LINE = GL_LINE,
		POINT = GL_POINT
	};

	void SetDrawMode(Mode m);

	void SetLineWidth(float width);

protected:

	friend class GraphicsObjectManager;

	GO3D(Model* const model);

	virtual ~GO3D();

	virtual void Update() override;

	struct MVP
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 projection;
	} mvp;

	glm::mat4 translation;

	glm::mat4 rotation;

	glm::mat4 scale;

	glm::mat4 transformation;

	Mode drawMode;

	unsigned int mvpBuffer;

	float lineWidth;

private:

	GO3D(const GO3D&) = delete;

	GO3D& operator=(const GO3D&) = delete;
	
	GO3D(GO3D&&) = delete;

	GO3D& operator=(GO3D&&) = delete;

};

#endif // GO3D_H