#include "LargePlane.h"

#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "GameEngine/Renderer/GraphicsObjects/GOTexturedLit.h"
#include "GameEngine/Terrain/Terrain.h"

LargePlane::LargePlane() :
	graphics(nullptr)
{
}

LargePlane::~LargePlane()
{
}

void LargePlane::Initialize()
{
	//graphics = GraphicsObjectManager::CreateGO3DTexturedLit(ModelManager::GetModel("LargePlane"), TextureManager::GetTexture("Grey"), TextureManager::GetTexture("Grey"));
	//graphics->Translate({ 0.0f, -0.5f, 0.0f });
	//graphics->SetShine(32.0f);

	
}

void LargePlane::Terminate()
{
	GraphicsObjectManager::Delete(graphics);
}

void LargePlane::GameUpdate()
{
}

void LargePlane::EditorUpdate()
{
}

void LargePlane::Load()
{
	if (!TextureManager::TextureLoaded("Grey"))
	{
		TextureManager::LoadTexture("Assets/Texture/Grey.png", "Grey");
	}

	if (!ModelManager::ModelLoaded("LargePlane"))
	{
		ModelManager::LoadModel("LargePlane", "Assets/Model/LargePlane.gltf");
	}
}

void LargePlane::Unload()
{
	if (TextureManager::TextureLoaded("Grey"))
	{
		TextureManager::UnloadTexture("Grey");
	}

	if (ModelManager::ModelLoaded("LargePlane"))
	{
		ModelManager::UnloadModel("LargePlane");
	}
}
