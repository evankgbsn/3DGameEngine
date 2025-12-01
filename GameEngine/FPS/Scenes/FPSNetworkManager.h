#ifndef FPSNETWORKMANAGER_H
#define FPSNETWORKMANAGER_H

#include "GameEngine/Scene/Scene.h"

#include <functional>

class FPSNetworkManager : public Scene
{
public:

	FPSNetworkManager();

	~FPSNetworkManager();

private:

	FPSNetworkManager(const FPSNetworkManager&) = delete;

	FPSNetworkManager& operator=(const FPSNetworkManager&) = delete;

	FPSNetworkManager(FPSNetworkManager&&) = delete;

	FPSNetworkManager& operator=(FPSNetworkManager&&) = delete;

	void Initialize() override;

	void Terminate() override;

	void Load() override;

	void Unload() override;

	std::function<void(const std::string& data)>* receiveLoginInfo;

	std::function<void(const std::string& data)>* receiveLoginServerResponse;

};


#endif // FPSNETWORKMANAGER_H