#ifndef GOTERRAIN_H
#define GOTERRAIN_H

#include "GO3D.h"
#include "GOLit.h"

class Model;
class Texture;

class GOTerrain : public GO3D, public GOLit
{
public:

	GOTerrain(Model* const model, const std::vector<Material>& textures);

	~GOTerrain();

	void Update() override;

	void RenderToShadowMap() override;

protected:

private:

	GOTerrain();

	GOTerrain(const GOTerrain&) = delete;

	GOTerrain& operator=(const GOTerrain&) = delete;

	GOTerrain(GOTerrain&&) = delete;

	GOTerrain& operator=(GOTerrain&&) = delete;

	unsigned int lightSpaceMatrixBuffer;

};

#endif // GOTERRAIN_H