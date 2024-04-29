#ifndef BOXES_H
#define BOXES_H

#include "GameEngine/GameObject/GameObject.h"

#include <vector>

class GOTexturedLit;
class OrientedBoundingBoxWithVisualization;

class Boxes : public GameObject
{

public:

	Boxes();

	~Boxes();

private:

	Boxes(const Boxes&) = delete;

	Boxes& operator=(const Boxes&) = delete;

	Boxes(Boxes&&) = delete;

	Boxes& operator=(Boxes&&) = delete;

	void Initialize() override;

	void Terminate() override;

	void Update() override;

	void Load() override;

	void Unload() override;

	std::vector<GOTexturedLit*> boxes;

	std::vector<OrientedBoundingBoxWithVisualization*> obbs;

};

#endif //BOXES_H