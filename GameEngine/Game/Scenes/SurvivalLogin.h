#ifndef SURVIVALLOGIN_H
#define SURVIVALLOGIN_H

#include "GameEngine/Scene/Scene.h"

class SurvivalLoginUI;

class SurvivalLogin : public Scene
{
public:

	SurvivalLogin();

	~SurvivalLogin();

	void Initialize() override;

	void Terminate() override;

private:

	SurvivalLoginUI* survivalLoginUI;

};

#endif // SURVIVALLOGIN_H