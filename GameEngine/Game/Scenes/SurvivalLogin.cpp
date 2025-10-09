#include "SurvivalLogin.h"

#include "../GameObjects/SurvivalLoginUI.h"

SurvivalLogin::SurvivalLogin() :
	survivalLoginUI(new SurvivalLoginUI())
{
}

SurvivalLogin::~SurvivalLogin()
{
	delete survivalLoginUI;
}

void SurvivalLogin::Initialize()
{
	RegisterGameObject(survivalLoginUI, "SurvivalLoginUI");
}

void SurvivalLogin::Terminate()
{
}
