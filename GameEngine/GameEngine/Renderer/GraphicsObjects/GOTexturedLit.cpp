#include "GOTexturedLit.h"

#include "../Shader/ShaderManager.h"
#include "../Texture/Texture.h"
#include "../Light/LightManager.h"
#include "../Light/DirectionalLight.h"
#include "../Light/PointLight.h"

GOTexturedLit::GOTexturedLit(Model* const model, Texture* const diffuseMap, Texture* const specularMap) :
	GOLit(diffuseMap, specularMap),
	GO3D(model)
{
	
}

GOTexturedLit::~GOTexturedLit()
{
	
}

void GOTexturedLit::Update()
{
	ShaderManager::StartShaderUsage("TexturedLit");

	GOLit::UpdateLighting();
	GO3D::Update();

	ShaderManager::EndShaderUsage("TexturedLit");
}
