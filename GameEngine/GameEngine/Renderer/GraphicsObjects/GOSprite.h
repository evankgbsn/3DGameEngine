#ifndef GOSPRITE_H
#define GOSPRITE_H

#include "GraphicsObject.h"

#include <glm/glm.hpp>

class Texture;

class Model;

class GOSprite : public GraphicsObject
{
public:

	GOSprite(Model* model, Texture* imageTexture, const glm::vec2& initialPosition);

	~GOSprite();

	glm::vec2 GetPosition() const;

	void SetPosition(const glm::vec2& newPosition);

	void Translate(const glm::vec2& translation);

	void SetTexture(Texture* const newTexture);

	void SetScale(float x, float y);

	void SetZ(float z);

	glm::vec2 GetScale() const;

	glm::mat4 GetProjection() const;

	glm::mat4 GetModelMat() const;

	Texture* const GetTexture() const;

protected:

private:

	GOSprite() = delete;

	GOSprite(const GOSprite&) = delete;

	GOSprite& operator=(const GOSprite&) = delete;

	GOSprite(GOSprite&&) = delete;

	GOSprite& operator=(GOSprite&&) = delete;

	void Update() override;

	unsigned int projectionBuffer;

	unsigned int positionBuffer;

	glm::mat4 projection;

	glm::mat4 modelMat;

	Texture* imageTexture;

};

#endif // GOSPRITE_H