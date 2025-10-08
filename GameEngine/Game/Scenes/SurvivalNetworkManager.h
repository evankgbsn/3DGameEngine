#ifndef SURVIVALNETWORKMANAGER_H
#define SURVIVALNETWORKMANAGER_H

#include "GameEngine/Scene/Scene.h"

#include <functional>

class SurvivalNetworkManager : public Scene
{
public:

	SurvivalNetworkManager();

	~SurvivalNetworkManager();

	void Initialize() override;

	void Terminate() override;

private:

	SurvivalNetworkManager(const SurvivalNetworkManager&) = delete;

	SurvivalNetworkManager& operator=(const SurvivalNetworkManager&) = delete;

	SurvivalNetworkManager(SurvivalNetworkManager&&) = delete;

	SurvivalNetworkManager& operator=(SurvivalNetworkManager&&) = delete;

	std::function<void(const std::string& data)>* receiveLoginInfo;

	std::function<void(const std::string& data)>* receiveLoginServerResponse;
};

#endif // SURVIVALNETWORKMANAGER_H