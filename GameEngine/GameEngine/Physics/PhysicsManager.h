#ifndef PHYSICSMANAGER_H
#define PHYSICSMANAGER_H

class PhysicsManager
{

public:

	static void Initialize();

	static void Terminate();

private:

	friend class SingletonHelpers;

	PhysicsManager();

	~PhysicsManager();

	static PhysicsManager* instance;
};


#endif // PHYSICSMANAGER_H