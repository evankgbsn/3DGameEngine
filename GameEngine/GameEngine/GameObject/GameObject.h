#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

class GameObject
{
public:

	GameObject();

	~GameObject();

	virtual void Update();

protected:



private:

	GameObject(const GameObject*) = delete;

	GameObject& operator=(const GameObject&) = delete;

	GameObject(GameObject&&) = delete;

	GameObject& operator=(GameObject&&) = delete;

};

#endif // GAMEOBJECT_H