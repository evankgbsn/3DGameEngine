#ifndef LARGEPLANE_H
#define LARGEPLANE_H

#include "GameEngine/GameObject/GameObject.h"

class GraphicsObjectTexturedLit;

class LargePlane : public GameObject
{

public:

	LargePlane();

	~LargePlane();

private:

	LargePlane(const LargePlane&) = delete;

	LargePlane& operator=(const LargePlane&) = delete;

	LargePlane(LargePlane&&) = delete;

	LargePlane& operator=(LargePlane&&) = delete;

	void Initialize() override;

	void Terminate() override;

	void GameUpdate() override;

	void EditorUpdate();

	void Load() override;

	void Unload() override;

	const std::vector<char> Serialize() const override;

	void Deserialize(const std::vector<char>& data) override;

	GraphicsObjectTexturedLit* graphics;

};

#endif // LARGEPLANE_H