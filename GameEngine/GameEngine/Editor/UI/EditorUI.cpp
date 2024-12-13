#include "EditorUI.h"

#include "../../UI/Button.h"
#include "../../Renderer/Texture/TextureManager.h"
#include "../../Editor/Editor.h"
#include "../../Renderer/Window/WindowManager.h"

EditorUI::EditorUI()
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

void EditorUI::CreatePlayButton()
{
	TextureManager::LoadTexture("Assets/Texture/Play.png", "Play");
	TextureManager::LoadTexture("Assets/Texture/PlayPress.png", "PlayPress");
	TextureManager::LoadTexture("Assets/Texture/PlayIdle.png", "PlayIdle");


	std::function<void()>* buttonFunc = new std::function<void()>([]() {
		Editor::Disbale();
		});

	Window* window = WindowManager::GetWindow("Engine");
	float windowHeight = (float)window->GetHeight();
	float windowWidth = (float)window->GetWidth();

	playButton = new Button("PlayIdle", "Play", "PlayPress", "Rectangle", { (windowWidth/2.0f) - 50.0f, windowHeight - 100.0f }, buttonFunc);
}
