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