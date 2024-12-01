#ifndef EDITOR_H
#define EDITOR_H

#include <functional>
#include <list>

class GOColored;

class Editor
{
public:

	static void Initialize();
	
	static void Terminate();

	static void Update();

	static bool Enabled();

	static void Disbale();

	static void Enable();

	static bool ShiftPressed();

	static float GetGridY();

	static void SetGridY(float newY);

	static void RegisterOnEditorEnable(std::function<void()>* function);

	static void DeregisterOnEditorEnable(std::function<void()>* function);

	static void RegisterOnEditorDisable(std::function<void()>* function);

	static void DeregisterOnEditorDisable(std::function<void()>* function);

private:

	friend class SingletonHelpers;

	Editor();

	~Editor();

	Editor(const Editor&) = delete;

	Editor& operator=(const Editor&) = delete;

	Editor(Editor&&) = delete;

	Editor& operator=(Editor&&) = delete;

	void SetupEditorInput();

	void InitializeGrid();

	void TerminateGrid();

	void FreeCameraMovement();

	static Editor* instance;

	GOColored* grid;

	float gridSpeed;

	bool enabled;

	bool shiftPressed;

	bool rightMousePressed;

	std::list<std::function<void()>*> onEditorEnableCallbacks;

	std::list<std::function<void()>*> onEditorDisableCallbacks;
};

#endif // EDITOR_H