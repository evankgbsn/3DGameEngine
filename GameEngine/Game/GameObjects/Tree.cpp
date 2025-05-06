#include "Tree.h"

#include "GameEngine/GameObject/Component/GraphicsObjectTexturedLit.h"
#include "GameEngine/GameObject/Component/StaticColliderComponent.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Renderer/Camera/CameraManager.h"
#include "GameEngine/Renderer/Window/WindowManager.h"
#include "GameEngine/Math/Shapes/Ray.h"

Tree::Tree() :
	GameObject("Tree"),
	graphics(nullptr),
	collider(nullptr)
{
	RegisterGameObjectClassType<Tree>(this);
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
	collider->Translate(newPos - graphics->GetPosition());
	graphics->SetPosition(newPos);
}

glm::vec3 Tree::GetPosition() const
{
	return graphics->GetPosition();
}

void Tree::Start()
{
	if (collider->IsVisible())
	{
		collider->ToggleVisibility();
	}
}

void Tree::End()
{
	if (!collider->IsVisible())
	{
		collider->ToggleVisibility();
	}
}

void Tree::Initialize()
{
	graphics = new GraphicsObjectTexturedLit(ModelManager::GetModel("Tree"), TextureManager::GetTexture("CrateTree"), TextureManager::GetTexture("GreyTree"));

	collider = new StaticColliderComponent(graphics);
}

void Tree::Terminate()
{
	delete collider;
	delete graphics;
}

void Tree::GameUpdate()
{
	collider->UpdateCollider();
}

void Tree::EditorUpdate()
{
	collider->UpdateCollider();
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

void Tree::SetRotation(const glm::mat4& rotation)
{
	graphics->SetRotation(rotation);
}

glm::mat4 Tree::GetRotation() const
{
	return graphics->GetRotation();
}

const std::vector<char> Tree::Serialize() const
{
	return std::vector<char>();
}

void Tree::Deserialize(const std::vector<char>& data)
{
}
