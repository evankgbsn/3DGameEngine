#ifndef GOTEXTUREDANIMATEDLIT_H
#define GOTEXTUREDANIMATEDLIT_H

#include "GO3DAnimated.h"
#include "GOLit.h"

class Texture;
class Animation;

class GOTexturedAnimatedLit : public GO3DAnimated, public GOLit
{

public:

	GOTexturedAnimatedLit(Model* const model, Texture* const diffuse, Texture* const specular);

	~GOTexturedAnimatedLit();

	void Update() override;

	void RenderToShadowMap() override;

	Texture* const GetDiffuseTexture() const;

	Texture* const GetSpecularTexture() const;

private:

	GOTexturedAnimatedLit(const GOTexturedAnimatedLit&) = delete;

	GOTexturedAnimatedLit& operator=(const GOTexturedAnimatedLit&) = delete;

	GOTexturedAnimatedLit(GOTexturedAnimatedLit&&) = delete;

	GOTexturedAnimatedLit& operator=(GOTexturedAnimatedLit&&) = delete;

	unsigned int lightSpaceMatrixBuffer;

	Texture* diffuse;

	Texture* specular;

};

#endif // GOTEXTUREDANIMATEDLIT_H