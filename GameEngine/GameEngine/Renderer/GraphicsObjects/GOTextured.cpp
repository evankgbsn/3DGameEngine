#include "GOTextured.h"

#include "../Shader/ShaderManager.h"
#include "../Texture/Texture.h"

#include "GL/glew.h"

GOTextured::GOTextured(Model* const model, Texture* const tex) :
	GO3D(model),
	texture(tex)
{
}

GOTextured::~GOTextured()
{
}

Texture* const GOTextured::GetTexture() const
{
	return texture;
}

void GOTextured::Update()
{
	ShaderManager::StartShaderUsage("Textured");

	texture->Bind(GL_TEXTURE0);

	GO3D::Update();

	ShaderManager::EndShaderUsage("Textured");
}
