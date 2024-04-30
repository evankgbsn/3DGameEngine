#include "Character.h"

#include "GameEngine/Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "GameEngine/Renderer/GraphicsObjects/GOTexturedAnimatedLit.h"
#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Renderer/Model/Model.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/Collision/AnimatedCollider.h"
#include "GameEngine/Collision/OrientedBoundingBoxWithVisualization.h"
#include "GameEngine/Input/InputManager.h"
#include "GameEngine/Scene/SceneManager.h"

Character::Character() :
	graphics(nullptr),
	collider(nullptr),
	toggleColliderVisibility(nullptr)
{
	toggleColliderVisibility = new std::function<void(int)>([this](int keyCode)
		{
			collider->ToggleVisibility();
		});

}

Character::~Character()
{
}

void Character::Initialize()
{
	graphics = GraphicsObjectManager::CreateGO3DTexturedAnimatedLit(ModelManager::GetModel("Woman"), TextureManager::GetTexture("Grey"), TextureManager::GetTexture("Random"));
	graphics->SetShine(32.0f);
	graphics->SetClip(7);
	graphics->Translate({ 0.0f, -0.5f, 0.0f });

	collider = new AnimatedCollider(graphics);

	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_V, toggleColliderVisibility, "CharacterColliderVisibility");

	obb = new OrientedBoundingBoxWithVisualization(ModelManager::GetModel("Woman")->GetVertices());
}

void Character::Terminate()
{
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_V, "CharacterColliderVisibility");

	delete collider;
	delete obb;

	GraphicsObjectManager::Delete(graphics);
}

void Character::Update()
{
	collider->Update();
	collider->Intersect(*obb);
}

void Character::Load()
{
	if (!TextureManager::TextureLoaded("Woman"))
	{
		TextureManager::LoadTexture("Assets/Texture/Woman.png", "Woman");
	}

	if (!TextureManager::TextureLoaded("Random"))
	{
		TextureManager::LoadTexture("Assets/Texture/container2_specular.png", "Random");
	}


	if (!ModelManager::ModelLoaded("Woman"))
	{
		ModelManager::LoadModel("Woman", "Assets/Model/Woman.gltf");
	}
}

void Character::Unload()
{
	if (TextureManager::TextureLoaded("Woman"))
	{
		TextureManager::UnloadTexture("Woman");
	}

	if (ModelManager::ModelLoaded("Woman"))
	{
		ModelManager::UnloadModel("Woman");
	}
}
