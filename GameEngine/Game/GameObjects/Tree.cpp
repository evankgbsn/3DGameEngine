#include "Tree.h"

#include "GameEngine/Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "GameEngine/Renderer/GraphicsObjects/GOTexturedLit.h"
#include "GameEngine/Renderer/GraphicsObjects/GOColored.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Collision/StaticCollider.h"
#include "GameEngine/Renderer/Camera/CameraManager.h"
#include "GameEngine/Renderer/Window/WindowManager.h"
#include "GameEngine/Math/Shapes/Ray.h"

Tree::Tree() :
	graphics(nullptr),
	collider(nullptr)
{
}

Tree::~Tree()
{
}

bool Tree::Hovered() const
{
	Camera& cam = CameraManager::GetActiveCamera();

	// Screen space to world space for object picking.
	Window* window = WindowManager::GetWindow("Engine");
	glm::vec2 cursorPos = window->GetCursorPosition();
	glm::mat4 invPersp = glm::inverse(cam.GetProjection());
	glm::mat4 invView = glm::inverse(cam.GetView());
	glm::mat4 screenToNDC(
		glm::vec4((float)window->GetWidth() / 2.0f, 0.0f, 0.0f, 0.0f),
		glm::vec4(0, -(float)window->GetHeight() / 2.0f, 0.0f, 0.0f),
		glm::vec4(0.0f, 0.0f, 0.5f, 0.0f),
		glm::vec4((float)window->GetWidth() / 2.0f, (float)window->GetHeight() / 2.0f, 0.5f, 1.0f)
	);
	screenToNDC = glm::inverse(screenToNDC);

	glm::vec4 x = glm::vec4(cursorPos.x, cursorPos.y, 1.0f, 1.0f);
	x = screenToNDC * x;
	x = invPersp * x;
	x = invView * x;
	x /= x.w;

	Ray lineFromScreenToWorld(cam.GetPosition(), glm::normalize(glm::vec3(x) - cam.GetPosition()));

	return collider->Intersect(lineFromScreenToWorld) != -1.0f;
}

void Tree::SetPosition(const glm::vec3& newPos)
{
	collider->Translate(newPos - graphics->GetTranslation());
	graphics->SetTranslation(newPos);
}

void Tree::Start()
{
	collider->ToggleVisibility();
}

void Tree::End()
{
	collider->ToggleVisibility();
}

void Tree::Initialize()
{
	graphics = GraphicsObjectManager::CreateGO3DTexturedLit(ModelManager::GetModel("Tree"), TextureManager::GetTexture("CrateTree"), TextureManager::GetTexture("GreyTree"));

	collider = new StaticCollider(graphics);
}

void Tree::Terminate()
{
	GraphicsObjectManager::Delete(graphics);
	delete collider;
}

void Tree::GameUpdate()
{
	collider->Update();
}

void Tree::EditorUpdate()
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

	if (!ModelManager::ModelLoaded("Cube"))
	{
		ModelManager::LoadModel("Cube", "Assets/Model/Cube.gltf");
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
