#include "GOTexturedAnimated.h"

#include "../Shader/ShaderManager.h"
#include "../Texture/Texture.h"
#include "../Animation/Animation.h"
#include "../Animation/Armature.h"
#include "../Model/Model.h"

#include <GL/glew.h>

GOTexturedAnimated::GOTexturedAnimated(Model* const model, Texture* const tex) :
	GO3DAnimated(model),
	texture(tex)
{
	
}

GOTexturedAnimated::~GOTexturedAnimated()
{
}

void GOTexturedAnimated::Update()
{
	ShaderManager::StartShaderUsage("TexturedAnimated");

	texture->Bind(GL_TEXTURE0);

	GO3DAnimated::Update();
	GO3D::Update();

	ShaderManager::EndShaderUsage("TexturedAnimated");
}

Texture* const GOTexturedAnimated::GetTexture() const
{
	return texture;
}
