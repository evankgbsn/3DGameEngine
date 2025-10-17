#ifndef EDITORUI_H
#define EDITORUI_H

#include <functional>

class Button;
class InputField;
class Sprite;
class Text;
class TextField;

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

	TextField* sceneManagementWindowTitle;

	TextField* sceneName;

	TextField* sceneFileName;

	TextField* sceneLoadingTitle;

	TextField* sceneUnloadingTitle;

	TextField* unloadSceneName;

	TextField* saveSceneTitle;

	TextField* saveSceneName;

	TextField* saveSceneFile;

	InputField* loadSceneInputField0;

	InputField* loadSceneInputField1;

	InputField* unloadSceneInputField;

	InputField* saveSceneInputFieldName;

	InputField* saveSceneInputFieldFile;

	Sprite* sceneManagementBackground;

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