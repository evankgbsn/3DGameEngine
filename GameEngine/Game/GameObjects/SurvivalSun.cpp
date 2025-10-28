#include "SurvivalSun.h"

#include "GameEngine/GameObject/Component/DirectionalLightComponent.h"

SurvivalSun::SurvivalSun() :
	GameObject(std::string("Sun")),
	sun(nullptr)
{
	RegisterGameObjectClassType<SurvivalSun>(this);
}

SurvivalSun::~SurvivalSun()
{
}

void SurvivalSun::Initialize()
{
	sun = new DirectionalLightComponent({-.5f, -.5f, -.5f}, {1.5f, .5f, 0.0f});
	AddComponent(sun, "Sun");

	sun->SetAmbientIntensity(0.5);
}

void SurvivalSun::Terminate()
{
	RemoveComponent("Sun");

	if (sun != nullptr)
	{
		delete sun;
	}
}

void SurvivalSun::GameUpdate()
{

}

void SurvivalSun::EditorUpdate()
{
}

void SurvivalSun::Load()
{
}

void SurvivalSun::Unload()
{
}
