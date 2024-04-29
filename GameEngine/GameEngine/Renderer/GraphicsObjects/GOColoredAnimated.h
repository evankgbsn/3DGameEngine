#ifndef GOCOLOREDANIMATED_H
#define GOCOLOREDANIMATED_H

#include "GO3DAnimated.h"

#include <glm/glm.hpp>

class Animation;

class GOColoredAnimated : public GO3DAnimated
{
public:

	GOColoredAnimated(Model* const model, glm::vec4 initialColor);

	~GOColoredAnimated();

	void Update() override;

	const glm::vec4& GetColor() const;

	void SetColor(const glm::vec4& newColor);

protected:

private:

	GOColoredAnimated(const GOColoredAnimated&) = delete;

	GOColoredAnimated& operator=(const GOColoredAnimated&) = delete;
	
	GOColoredAnimated(GOColoredAnimated&&) = delete;

	GOColoredAnimated& operator=(GOColoredAnimated&&) = delete;

	glm::vec4 color;

	unsigned int colorBuffer;
};

#endif // GOCOLOREDANIMATED_H