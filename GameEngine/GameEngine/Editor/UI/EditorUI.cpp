#include "EditorUI.h"

#include "../../UI/Button.h"
#include "../../Renderer/Texture/TextureManager.h"
#include "../../Editor/Editor.h"
#include "../../Renderer/Window/WindowManager.h"
#include "../../Renderer/Model/ModelManager.h"

EditorUI::EditorUI() :
	disabled(false)
{
	CreatePlayButton();
}

EditorUI::~EditorUI()
{
	delete playButton;
}

void EditorUI::Update()
{
	playButton->Update();
}

bool EditorUI::IsDisabled() const
{
	return disabled;
}

void EditorUI::CreatePlayButton()
{
	TextureManager::LoadTexture("Assets/Texture/grey.png", "Play");
	TextureManager::LoadTexture("Assets/Texture/Green.png", "PlayPress");
	TextureManager::LoadTexture("Assets/Texture/Green.png", "PlayIdle");
	ModelManager::LoadModel("Play", "Assets/Model/PlayButton.gltf");

	static std::function<void()> buttonFunc = std::function<void()>([this]() 
		{
			Editor::Disbale();
		});

	Window* window = WindowManager::GetWindow("Engine");
	float windowHeight = (float)window->GetHeight();
	float windowWidth = (float)window->GetWidth();

	playButton = new Button("PlayIdle", "Play", "PlayPress", "Play", { (windowWidth/2.0f) - 50.0f, windowHeight - 100.0f }, &buttonFunc);
}

void EditorUI::Disable()
{
	disabled = true;
	playButton->Disable();
}

void EditorUI::Enable()
{
	playButton->Enable();
	disabled = false;
}
