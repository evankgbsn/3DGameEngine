#ifndef MAINSCENE_H
#define MAINSCENE_H

#include "GameEngine/Scene/SceneManager.h"
#include "GameEngine/Scene/Scene.h"

#include <functional>


class LightArray;
class Boxes;
class Tree;
class LargePlane;
class DirectionalLightGameObject;
class Character;
class FreeCamera;

class MainScene : public Scene
{

public:

	MainScene();

	~MainScene();

private:

	MainScene(const MainScene&) = delete;

	MainScene& operator=(const MainScene&) = delete;

	MainScene(MainScene&&) = delete;

	MainScene& operator=(MainScene&&) = delete;

	void Initialize() override;

	void Terminate() override;

	LightArray* spotLights;

	DirectionalLightGameObject* directionalLight;

	Character* character;

	Tree* tree;

	LargePlane* groundPlane;

	FreeCamera* cam;

	std::function<void(int)>* loadSub;

	std::function<void(int)>* unloadSub;

};

#endif // MAINSCENE_H