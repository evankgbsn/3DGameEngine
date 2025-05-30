#ifndef GOTEXTUREDLIT_H
#define GOTEXTUREDLIT_H

#include "GO3D.h"
#include "GOLit.h"

class Model;
class Texture;

class GOTexturedLit : public GO3D, public GOLit
{
public:

	GOTexturedLit(Model* const model, Texture* const diffuseMap, Texture* const specularMap);

	~GOTexturedLit();

	void Update() override;

	void RenderToShadowMap() override;

	Texture* const GetDiffuseTexture() const;

	Texture* const GetSpecularTexture() const;

protected:

private:

	GOTexturedLit();

	GOTexturedLit(const GOTexturedLit&) = delete;

	GOTexturedLit& operator=(const GOTexturedLit&) = delete;

	GOTexturedLit(GOTexturedLit&&) = delete;

	GOTexturedLit& operator=(GOTexturedLit&&) = delete;

	unsigned int lightSpaceMatrixBuffer;

};

#endif // GOTEXTUREDLIT_H