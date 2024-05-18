#ifndef CHARACTER_H
#define CHARACTER_H

#include "GameEngine/GameObject/GameObject.h"

#include <functional>

class GOTexturedAnimatedLit;
class AnimatedCollider;
class OrientedBoundingBoxWithVisualization;
class GOTexturedLit;
class StaticCollider;

class Character : public GameObject
{

public:

	Character();

	~Character();

private:

	Character(const Character&) = delete;

	Character& operator=(const Character&) = delete;

	Character(Character&&) = delete;

	Character& operator=(Character&&) = delete;

	void Initialize() override;

	void Terminate() override;

	void Update() override;

	void Load() override;

	void Unload() override;

	GOTexturedAnimatedLit* graphics;

	GOTexturedAnimatedLit* graphics2;

	GOTexturedLit* homeGraphics;

	GOTexturedLit* treeGraphics;

	StaticCollider* treeCollider;

	AnimatedCollider* collider;

	AnimatedCollider* collider2;

	OrientedBoundingBoxWithVisualization* obb;

	std::function<void(int)>* toggleColliderVisibility;

	std::function<void(int)>* castLine;

	std::function<void(int)>* forward;
	std::function<void(int)>* backward;
	std::function<void(int)>* left;
	std::function<void(int)>* right;

};

#endif // CHARACTER_H