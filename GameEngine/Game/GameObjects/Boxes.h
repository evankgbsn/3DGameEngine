#ifndef BOXES_H
#define BOXES_H

#include "GameEngine/GameObject/GameObject.h"

#include <vector>

class GraphicsObjectTexturedLit;
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

	void GameUpdate() override;

	void EditorUpdate() override;

	void Load() override;

	void Unload() override;

	void Serialize() override;

	void Deserialize() override;

	std::vector<GraphicsObjectTexturedLit*> boxes;

	std::vector<OrientedBoundingBoxWithVisualization*> obbs;

};

#endif //BOXES_H