#ifndef SUBSCENE_H
#define SUBSCENE_H

#include "GameEngine/Scene/Scene.h"

#include <functional>

class Boxes;
class FreeCamera;

class Subscene : public Scene
{

public:

	Subscene();

	~Subscene();

private:

	Subscene(const Subscene&) = delete;

	Subscene& operator=(const Subscene&) = delete;

	Subscene(Subscene&&) = delete;

	Subscene& operator=(Subscene&&) = delete;

	void Initialize() override;

	void Terminate() override;

	Boxes* boxes;

	FreeCamera* cam;

	std::function<void(int)>* loadMain;
};

#endif // SUBSCENE_H