#include "Boxes.h"

#include "GameEngine/Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "GameEngine/Renderer/GraphicsObjects/GOTexturedLit.h"
#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/Time/TimeManager.h"
#include "GameEngine/Collision/OrientedBoundingBoxWithVisualization.h"	
#include "GameEngine/Renderer/Model/Model.h"

Boxes::Boxes()
{
}

Boxes::~Boxes()
{
}

void Boxes::Initialize()
{
	float count = 10;
	float dist = 5.0f;
	for (unsigned int i = 0; i < count; ++i)
	{
		for (unsigned int j = 0; j < count; ++j)
		{
			GOTexturedLit* box = GraphicsObjectManager::CreateGO3DTexturedLit(ModelManager::GetModel("Cube"), TextureManager::GetTexture("Crate"), TextureManager::GetTexture("CrateSpecular"));
			box->SetShine(32.0f);
			box->SetTranslation({ dist * i, 1.5f, dist * j });
			boxes.push_back(box);
			obbs.push_back(new OrientedBoundingBoxWithVisualization(ModelManager::GetModel("Cube")->GetVertices()));

			
		}
	}
}

void Boxes::Terminate()
{
	for (GOTexturedLit* box : boxes)
	{
		GraphicsObjectManager::Delete(box);
	}

	for (OrientedBoundingBoxWithVisualization* obb : obbs)
	{
		delete obb;
	}

	obbs.clear();
	boxes.clear();
}

void Boxes::Update()
{
	float rotationSpeed = 10.0f;

	unsigned int i = 0;
	for (GOTexturedLit* box : boxes)
	{
		box->Rotate(rotationSpeed * TimeManager::DeltaTime(), { 0.0f, 1.0f, 0.0f });
		box->Rotate(rotationSpeed * TimeManager::DeltaTime(), { 1.0f, 0.0f, 0.0f });
		obbs[i]->Update(box->GetTransform());
		i++;
	}
}

void Boxes::Load()
{
	if (!ModelManager::ModelLoaded("Cube"))
	{
		ModelManager::LoadModel("Cube", "Assets/Model/Cube.gltf");
	}

	if (!TextureManager::TextureLoaded("Crate"))
	{
		TextureManager::LoadTexture("./Assets/Texture/container2.png", "Crate");
	}

	if (!TextureManager::TextureLoaded("CrateSpecular"))
	{
		TextureManager::LoadTexture("./Assets/Texture/container2_specular.png", "CrateSpecular");
	}
}

void Boxes::Unload()
{
	if (ModelManager::ModelLoaded("Cube"))
	{
		ModelManager::UnloadModel("Cube");
	}

	if (TextureManager::TextureLoaded("Crate"))
	{
		TextureManager::UnloadTexture("Crate");
	}

	if (TextureManager::TextureLoaded("CrateSpecular"))
	{
		TextureManager::UnloadTexture("CrateSpecular");
	}

	// Unload model.
}
