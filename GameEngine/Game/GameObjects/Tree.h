#ifndef TREE_H
#define TREE_H

#include "GameEngine/GameObject/GameObject.h"

class GOTexturedLit;
class GOColored;

class Tree : public GameObject
{

public:

	Tree();

	~Tree();

private:

	Tree(const Tree&) = delete;

	Tree& operator=(const Tree&) = delete;

	Tree(Tree&&) = delete;

	Tree& operator=(Tree&&) = delete;

	void Initialize() override;

	void Terminate() override;

	void Update() override;

	void Load() override;

	void Unload() override;

	GOTexturedLit* graphics;

	GOColored* fakeCollider;

};

#endif // TREE_H