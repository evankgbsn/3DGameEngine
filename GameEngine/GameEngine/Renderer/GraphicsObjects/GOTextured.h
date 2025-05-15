#ifndef GOTEXTURED_H
#define GOTEXTURED_H

#include "GO3D.h"

#include <glm/glm.hpp>

class Texture;

class GOTextured : public GO3D
{
public:

	GOTextured(Model* const model, Texture* const texture);

	~GOTextured();

	Texture* const GetTexture() const;

	void Update() override;

protected:

	Texture* texture;

private:

	struct MVP
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 projection;
	};

};

#endif // GOTEXTURED_H