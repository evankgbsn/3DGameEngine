#ifndef EDITOR_H
#define EDITOR_H

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

	static Editor* instance;

	GOColored* grid;

	float gridSpeed;

	bool enabled;

	bool shiftPressed;
};

#endif // EDITOR_H