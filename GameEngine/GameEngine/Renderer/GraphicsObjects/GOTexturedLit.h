#ifndef GOTEXTUREDLIT_H
#define GOTEXTUREDLIT_H

#include "GO3D.h"

class Model;
class Texture;

class GOTexturedLit : public GO3D
{
public:

	GOTexturedLit(Model* const model, Texture* const texture);

	~GOTexturedLit();

	void Update() override;

protected:

private:

	GOTexturedLit();

	GOTexturedLit(const GOTexturedLit&) = delete;

	GOTexturedLit& operator=(const GOTexturedLit&) = delete;

	GOTexturedLit(GOTexturedLit&&) = delete;

	GOTexturedLit& operator=(GOTexturedLit&&) = delete;

	Texture* texture;

	struct DirectionalLightUBO {
		glm::vec4 color;
		glm::vec4 direction;
	} directionalLight;

	unsigned int directionalLightBuffer;

	struct PointLightUBO {
		glm::vec4 color;
		glm::vec4 position;
	} pointLight;

	unsigned int pointLightBuffer;

	float ambientStrength;

	unsigned int ambientStrengthBuffer;

};

#endif // GOTEXTUREDLIT_H