#include "MainScene.h"

#include "../GameObjects/LargePlane.h"
#include "../GameObjects/LightArray.h"
#include "../GameObjects/Character.h"
#include "../GameObjects/Tree.h"
#include "../GameObjects/DirectionalLightGameObject.h"
#include "../GameObjects/Boxes.h"
#include "../GameObjects/FreeCamera.h"

MainScene::MainScene() :
	spotLights(new LightArray()),
	directionalLight(new DirectionalLightGameObject()),
	character(new Character()),
	tree(new Tree()),
	boxes(new Boxes()),
	groundPlane(new LargePlane()),
	cam(new FreeCamera())
{
}

MainScene::~MainScene()
{
	delete cam;
	delete spotLights;
	delete directionalLight;
	delete character;
	delete tree;
	delete boxes;
	delete groundPlane;
}

void MainScene::Initialize()
{
	RegisterGameObject(cam, "FreeCam");
	RegisterGameObject(boxes, "Boxes");
	RegisterGameObject(directionalLight, "DirectionalLight");
	RegisterGameObject(groundPlane, "Ground");
	RegisterGameObject(character, "Character");
	RegisterGameObject(tree, "Tree");
	//RegisterGameObject(spotLights, "Lights");
}

void MainScene::Terminate()
{
	//DeregisterGameObject("Lights");
	DeregisterGameObject("Tree");
	DeregisterGameObject("FreeCam");
	DeregisterGameObject("Boxes");
	DeregisterGameObject("DirectionalLight");
	DeregisterGameObject("Ground");
	DeregisterGameObject("Character");
}