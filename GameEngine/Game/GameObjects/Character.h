#ifndef CHARACTER_H
#define CHARACTER_H

#include "GameEngine/GameObject/GameObject.h"

#include <functional>

class GOTexturedAnimatedLit;
class AnimatedCollider;
class OrientedBoundingBoxWithVisualization;
class GOTexturedLit;

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

	GOTexturedLit* homeGraphics;

	AnimatedCollider* collider;

	OrientedBoundingBoxWithVisualization* obb;

	std::function<void(int)>* toggleColliderVisibility;

	std::function<void(int)>* castLine;

};

#endif // CHARACTER_H