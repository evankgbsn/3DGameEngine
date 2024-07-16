#ifndef EDITOR_H
#define EDITOR_H

class Editor
{
public:

	static void Initialize();
	
	static void Terminate();

	static void Update();

	static bool Enabled();

	static void Disbale();

	static void Enable();

private:

	friend class SingletonHelpers;

	Editor();

	~Editor();

	Editor(const Editor&) = delete;

	Editor& operator=(const Editor&) = delete;

	Editor(Editor&&) = delete;

	Editor& operator=(Editor&&) = delete;

	void SetupEditorInput();

	static Editor* instance;

	bool enabled;
};

#endif // EDITOR_H