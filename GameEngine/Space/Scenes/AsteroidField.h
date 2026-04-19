#ifndef ASTEROIDFIELD_H
#define ASTEROIDFIELD_H

#include "GameEngine/Scene/Scene.h"

class Asteroid;
class NetworkObject;
class ServerFreeCamera;
class PlayerShip;
class MainDirectionalLight;

class AsteroidField : public Scene
{
public:

	AsteroidField();

	~AsteroidField();

private:

	void Initialize() override;

	void Terminate() override;

	void Load() override;

	void Unload() override;

	void Start() override;

	void End() override;

	AsteroidField(const AsteroidField&) = delete;

	AsteroidField& operator=(const AsteroidField&) = delete;

	AsteroidField(AsteroidField&&) = delete;

	AsteroidField& operator=(AsteroidField&&) = delete;

	Asteroid* asteroid;

	NetworkObject* localPlayer;

	ServerFreeCamera* serverFreeCam;

	PlayerShip* player;

	MainDirectionalLight* mainLight;
};

#endif // ASTEROIDFIELD_H