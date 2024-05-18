#include "Tree.h"

#include "GameEngine/Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "GameEngine/Renderer/GraphicsObjects/GOTexturedLit.h"
#include "GameEngine/Renderer/GraphicsObjects/GOColored.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Collision/StaticCollider.h"

Tree::Tree() :
	graphics(nullptr)
{
}

Tree::~Tree()
{
}

void Tree::Initialize()
{
	graphics = GraphicsObjectManager::CreateGO3DTexturedLit(ModelManager::GetModel("Tree"), TextureManager::GetTexture("CrateTree"), TextureManager::GetTexture("GreyTree"));
	graphics->Scale({ 2.0f, 2.0f, 2.0f });
	graphics->SetTranslation({ 10.5f, -1.0f, 10.5f });

	collider = new StaticCollider(graphics);
}

void Tree::Terminate()
{
	GraphicsObjectManager::Delete(graphics);
	delete collider;
}

void Tree::Update()
{
	collider->Update();
}

void Tree::Load()
{
	if (!ModelManager::ModelLoaded("Tree"))
	{
		ModelManager::LoadModel("Tree", "Assets/Model/Tree.gltf");
	}

	if (!TextureManager::TextureLoaded("CrateTree"))
	{
		TextureManager::LoadTexture("Assets/Texture/container2.png", "CrateTree");
	}

	if (!TextureManager::TextureLoaded("GreyTree"))
	{
		TextureManager::LoadTexture("Assets/Texture/Grey.png", "GreyTree");
	}

}

void Tree::Unload()
{
	if (ModelManager::ModelLoaded("Tree"))
	{
		ModelManager::UnloadModel("Tree");
	}

	if (TextureManager::TextureLoaded("CrateTree"))
	{
		TextureManager::UnloadTexture("CrateTree");
	}

	if (TextureManager::TextureLoaded("GreyTree"))
	{
		TextureManager::UnloadTexture("GreyTree");
	}
}
