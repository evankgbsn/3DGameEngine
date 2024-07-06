#ifndef EDITOR_H
#define EDITOR_H

class Editor
{
public:

	static void Initialize();
	
	static void Terminate();

private:

	friend class SingletonHelpers;

	Editor();

	~Editor();

	Editor(const Editor&) = delete;

	Editor& operator=(const Editor&) = delete;

	Editor(Editor&&) = delete;

	Editor& operator=(Editor&&) = delete;
};

#endif // EDITOR_H