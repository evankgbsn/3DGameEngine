#ifndef TEST_H
#define TEST_H

#include "GameEngine/Scene/Scene.h"

class PlaneObj;
class FPSPlayer;
class Sun;
class Crate;
class ServerFreeCamera;
class NetworkObject;
class AK12Bullet;
class ShippingContainer;
class PointLightObject;
class WarehouseDoorway;
class WarehouseBrickWall;
class WarehouseWindowWall;
class WarehouseHorizontalBeam;
class WarehouseVerticalBeam;
class WarehouseVerticalBeamCorner;
class WarehouseRoofWall;
class WarehouseRoofHorizontalBeam;
class WarehouseVerticalBeamHalf;
class WarehouseRoofPannel;
class SpotLightObject;
class SpawnPoint;
class SkySphere;
class TestSoundObj;

class Test : public Scene
{

public:

	Test();

	~Test();

private:

	Test(const Test&) = delete;

	Test& operator=(const Test&) = delete;

	Test(Test&&) = delete;

	Test& operator=(Test&&) = delete;

	void Initialize() override;

	void Terminate() override;

	void Load() override;

	void Unload() override;

	void Start() override;

	void End() override;

	FPSPlayer* player;

	AK12Bullet* bullet;
	
	Sun* sun;
	
	Crate* crate;
	
	ServerFreeCamera* serverFreeCam;

	NetworkObject* localPlayer;

	PlaneObj* plane;

	ShippingContainer* shippingContainer;

	WarehouseDoorway* warehouseDoorway;

	WarehouseBrickWall* warehouseBrickWall;

	WarehouseWindowWall* warehouseWindowWall;

	WarehouseHorizontalBeam* warehouseHorizontalBeam;

	WarehouseVerticalBeam* warehouseVerticalBeam;

	WarehouseVerticalBeamCorner* warehouseVerticalBeamCorner;

	WarehouseRoofWall* warehouseRoofWall;

	WarehouseRoofHorizontalBeam* warehouseRoofHorizontalBeam;

	WarehouseVerticalBeamHalf* warehouseVerticalBeamHalf;

	WarehouseRoofPannel* warehouseRoofPannel;

	SpotLightObject* spotLight;

	SpawnPoint* spawnPoint;

	SkySphere* sky;

	TestSoundObj* sound;
};

#endif // TEST_H