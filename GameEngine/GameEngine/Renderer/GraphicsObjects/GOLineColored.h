#ifndef GOLINECOLORED_H
#define GOLINECOLORED_H

#include "GraphicsObject.h"

#include <glm/glm.hpp>

class GOLineColored : public GraphicsObject
{
public:

	GOLineColored(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color);

	~GOLineColored();

	void Update() override;

	void SetColor(const glm::vec4& color);

	glm::vec4 GetColor() const;

	void SetLineWidth(float width);

	float GetLineWidth() const;

	void SetStart(const glm::vec3& point0);

	void SetEnd(const glm::vec3& point1);

	glm::vec3 GetStart() const;

	glm::vec3 GetEnd() const;

private:

	GOLineColored() = delete;

	GOLineColored(const GOLineColored&) = delete;

	GOLineColored& operator=(const GOLineColored&) = delete;

	GOLineColored(GOLineColored&&) = delete;

	GOLineColored& operator=(GOLineColored&&) = delete;

	static unsigned int lineModelId;

	struct MVP
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 projection;
	} mvp;

	glm::vec4 color;

	glm::vec3 start;

	glm::vec3 end;

	unsigned int mvpBuffer;

	unsigned int colorBuffer;

	float lineWidth;
};

#endif // GOLINECOLORED_H