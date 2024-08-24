#ifndef SELECTIONMANAGER_H
#define SELECTIONMANAGER_H

class GameObject;

class SelectionManager
{

public:

	static void Initialize();

	static void Terminate();

	static void Update();

	static GameObject* const GetSelection();

private:

	friend class SingletonHelpers;

	SelectionManager();

	~SelectionManager();

	SelectionManager(const SelectionManager&) = delete;

	SelectionManager& operator=(const SelectionManager&) = delete;

	SelectionManager(SelectionManager&&) = delete;

	SelectionManager& operator=(SelectionManager&&) = delete;

	void SetupInput();

	static SelectionManager* instance;

	GameObject* selection;

};


#endif // SELECTIONAMANAGER_H