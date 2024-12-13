#ifndef EDITORUI_H
#define EDITORUI_H

class Button;

class EditorUI
{
public:

	EditorUI();

	~EditorUI();

	void Update();

private:

	EditorUI(const EditorUI&) = delete;

	EditorUI& operator=(const EditorUI&) = delete;

	EditorUI(EditorUI&&) = delete;

	EditorUI& operator=(EditorUI&&) = delete;

	void CreatePlayButton();

	Button* playButton;
};

#endif //EDITORUI_H