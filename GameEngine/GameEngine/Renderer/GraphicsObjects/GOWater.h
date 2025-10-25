#ifndef GOWATER_H
#define GOWATER_H

#include "GO3D.h"

class Model;

class GOWater : public GO3D
{

public:

	GOWater(Model* const model);

	~GOWater();

	void Update() override;

private:

	GOWater() = delete;

	GOWater(const GOWater&) = delete;

	GOWater& operator=(const GOWater&) = delete;

	GOWater(GOWater&&) = delete;

	GOWater& operator=(GOWater&&) = delete;

	unsigned int effectsBuffer;

	unsigned int camPositionBuffer;

	glm::vec4 camPosition;

	struct Effects
	{
		glm::vec4 tint;

		float tintStrength;

		float moveFactor;

		float waveStrength;

		float reflectionStrength;
		
	} effects;

};

#endif //GOWATER_H