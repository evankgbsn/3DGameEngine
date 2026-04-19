#ifndef SPACENETWORKMANAGER_H
#define SPACENETWORKMANAGER_H

#include "GameEngine/Scene/Scene.h"

#include <functional>

class SpaceNetworkManager : public Scene
{
public:

	SpaceNetworkManager();

	~SpaceNetworkManager();

private:

	SpaceNetworkManager(const SpaceNetworkManager&) = delete;

	SpaceNetworkManager& operator=(const SpaceNetworkManager&) = delete;

	SpaceNetworkManager(SpaceNetworkManager&&) = delete;

	SpaceNetworkManager& operator=(SpaceNetworkManager&&) = delete;

	void Initialize() override;

	void Terminate() override;

	void Load() override;

	void Unload() override;

	void Start() override;

	void End() override;

	std::function<void(const std::string& data)>* receiveLoginInfo;

	std::function<void(const std::string& data)>* receiveLoginServerResponse;

};

#endif // SPACENETWORKMANAGER_H