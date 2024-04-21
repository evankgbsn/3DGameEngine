#ifndef GOTEXTUREDANIMATEDLIT_H
#define GOTEXTUREDANIMATEDLIT_H

#include "GO3DAnimated.h"

class Texture;
class Animation;

class GOTexturedAnimatedLit : public GO3DAnimated
{

public:

	GOTexturedAnimatedLit(Model* const model, Texture* const texture);

	~GOTexturedAnimatedLit();

	void Update() override;

private:

	GOTexturedAnimatedLit(const GOTexturedAnimatedLit&) = delete;

	GOTexturedAnimatedLit& operator=(const GOTexturedAnimatedLit&) = delete;

	GOTexturedAnimatedLit(GOTexturedAnimatedLit&&) = delete;

	GOTexturedAnimatedLit& operator=(GOTexturedAnimatedLit&&) = delete;

	Texture* texture;

	struct DirectionalLightUBO
	{
		glm::vec4 color;
		glm::vec4 direction;
	} directionalLight;

	unsigned int directionalLightBuffer;
	
	struct PointLightUBO
	{
		glm::vec4 color;
		glm::vec4 position;
	} pointLight;

	unsigned int pointLightBuffer;

	float ambientLight;

	unsigned int ambientLightBuffer;

	unsigned int viewPositionBuffer;
};

#endif // GOTEXTUREDANIMATEDLIT_H