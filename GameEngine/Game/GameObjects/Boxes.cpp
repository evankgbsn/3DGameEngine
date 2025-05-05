#include "Boxes.h"

#include "GameEngine/GameObject/Component/GraphicsObjectTexturedLit.h"
#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/Time/TimeManager.h"
#include "GameEngine/Collision/OrientedBoundingBoxWithVisualization.h"	
#include "GameEngine/Renderer/Model/Model.h"

#include "GameEngine/GameObject/Component/DirectionalLightComponent.h"

Boxes::Boxes() :
	GameObject("Boxes")
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
			GraphicsObjectTexturedLit* box = new GraphicsObjectTexturedLit(ModelManager::GetModel("Cube"), TextureManager::GetTexture("Crate"), TextureManager::GetTexture("CrateSpecular"));
			box->SetShine(32.0f);
			box->SetPosition({ dist * i, 1.5f, dist * j });
			boxes.push_back(box);
			obbs.push_back(new OrientedBoundingBoxWithVisualization(ModelManager::GetModel("Cube")->GetVertices()));
			AddComponent(box, std::string("Box") + std::to_string(i+j));
		}
	}
}

void Boxes::Terminate()
{
	for (GraphicsObjectTexturedLit* box : boxes)
	{
		delete box;
	}

	for (OrientedBoundingBoxWithVisualization* obb : obbs)
	{
		delete obb;
	}

	obbs.clear();
	boxes.clear();
}

void Boxes::GameUpdate()
{
	float rotationSpeed = 10.0f;

	unsigned int i = 0;
	for (GraphicsObjectTexturedLit* box : boxes)
	{
		box->Rotate(rotationSpeed * TimeManager::DeltaTime(), { 0.0f, 1.0f, 0.0f });
		box->Rotate(rotationSpeed * TimeManager::DeltaTime(), { 1.0f, 0.0f, 0.0f });
		obbs[i]->Update(box->GetTransform());
		i++;
	}
}

void Boxes::EditorUpdate()
{
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

const std::vector<char> Boxes::Serialize() const
{
	return std::vector<char>();
}

void Boxes::Deserialize(const std::vector<char>& data)
{
}
