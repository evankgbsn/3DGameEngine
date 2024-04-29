#ifndef CHARACTER_H
#define CHARACTER_H

#include "GameEngine/GameObject/GameObject.h"

#include <functional>

class GOTexturedAnimatedLit;
class AnimatedCollider;

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

	AnimatedCollider* collider;

	std::function<void(int)>* toggleColliderVisibility;

};

#endif // CHARACTER_H