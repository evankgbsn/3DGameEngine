#ifndef EDITORUI_H
#define EDITORUI_H

#include <functional>

class Button;
class InputField;
class GOSprite;
class Text;

class EditorUI
{
public:

	EditorUI();

	~EditorUI();

	void Update();

	bool IsDisabled() const;

	void Disable();

	void Enable();

private:

	EditorUI(const EditorUI&) = delete;

	EditorUI& operator=(const EditorUI&) = delete;

	EditorUI(EditorUI&&) = delete;

	EditorUI& operator=(EditorUI&&) = delete;

	void CreatePlayButton();

	void CreateLoadSceneInputFields();

	void CreateSceneManagementInterface();

	void UpdateSceneManagementInterface();

	void CleanupSceneManagementInterface();

	void EnableSceneManagementInterface();

	void DisableSceneManagementInterface();

	void CreateInterfaceToggleCallbacks();

	void CleanupInterfaceToggleCallbacks();

	Button* playButton;

	Text* sceneManagementWindowTitle;

	Text* sceneName;

	Text* sceneFileName;

	Text* sceneLoadingTitle;

	Text* sceneUnloadingTitle;

	Text* unloadSceneName;

	Text* saveSceneTitle;

	Text* saveSceneName;

	Text* saveSceneFile;

	InputField* loadSceneInputField0;

	InputField* loadSceneInputField1;

	InputField* unloadSceneInputField;

	InputField* saveSceneInputFieldName;

	InputField* saveSceneInputFieldFile;

	GOSprite* sceneManagementBackground;

	std::function<void()>* loadSceneInputFieldOnEnter;

	std::function<void()>* unloadSceneInputFieldOnEnter;

	std::function<void()>* saveSceneInputFieldOnEnter;

	std::function<void(int)>* altPress;

	std::function<void(int)>* altRelease;

	std::function<void(int)>* sPress;

	bool disabled;

	bool altPressed;

	bool sceneManagementInterfaceEnabled;
};

#endif //EDITORUI_H