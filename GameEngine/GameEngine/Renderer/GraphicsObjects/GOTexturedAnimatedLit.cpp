#include "GOTexturedAnimatedLit.h"

#include "../Shader/ShaderManager.h"
#include "../Light/LightManager.h"
#include "../Light/DirectionalLight.h"
#include "../Light/PointLight.h"
#include "../Texture/Texture.h"
#include "../Model/Model.h"
#include "../Animation/Animation.h"
#include "../Animation/Armature.h"
#include "../Camera/Camera.h"
#include "../Camera/CameraManager.h"

#include <GL/glew.h>

GOTexturedAnimatedLit::GOTexturedAnimatedLit(Model* const model, Texture* const diffuseMap, Texture* const specularMap) :
	GOLit(diffuseMap, specularMap),
	GO3DAnimated(model)
{
}

GOTexturedAnimatedLit::~GOTexturedAnimatedLit()
{
}

void GOTexturedAnimatedLit::Update()
{
	ShaderManager::StartShaderUsage("TexturedAnimatedLit");

	GOLit::UpdateLighting();
	GO3DAnimated::Update();
	GO3D::Update();

	ShaderManager::EndShaderUsage("TexturedAnimatedLit");
}
