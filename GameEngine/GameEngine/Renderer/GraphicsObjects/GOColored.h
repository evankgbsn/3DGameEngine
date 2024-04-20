#ifndef GOTEST_H
#define GOTEST_H

#include "GO3D.h"

#include <glm/glm.hpp>

class Model;

class GOColored : public GO3D
{

public:

	GOColored(Model* const model, const glm::vec4& initialColor);

	~GOColored();

	void SetColor(const glm::vec4& color);

	const glm::vec4& GetColor() const;

	void Update() override;

private:

	GOColored() = delete;

	glm::vec4 color;

	unsigned int colorBuffer;

};

#endif // GOTEST_H