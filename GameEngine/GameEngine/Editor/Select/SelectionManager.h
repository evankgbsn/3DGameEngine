#ifndef SELECTIONMANAGER_H
#define SELECTIONMANAGER_H

#include <glm/glm.hpp>

class GameObject;
class Text;

class SelectionManager
{

public:

	static void Initialize();

	static void Terminate();

	static void Update();

	static GameObject* const GetSelection();

	static void ClearSelection();

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

	Text* selectionName;

	glm::vec3 selectionPlanePointDelta;

};


#endif // SELECTIONAMANAGER_H