#include "TimeManager.h"

#include "../Utils/Logger.h"

TimeManager* TimeManager::instance = nullptr;

void TimeManager::Initialize()
{
	if (instance == nullptr)
	{
		instance = new TimeManager();
		Logger::Log(std::string("Initialized TimeManager."), Logger::Category::Success);
	}
	else
	{
		Logger::Log(std::string("Calling TimeManager::Initialize() before TimeManager::Terminate()"), Logger::Category::Warning);
	}
}

void TimeManager::Terminate()
{
	if (instance != nullptr)
	{
		delete instance;
		Logger::Log(std::string("Terminated TimeManager."), Logger::Category::Success);
	}
	else
	{
		Logger::Log(std::string("Calling TimeManager::Terminate() before TimeManager::Initialize()"), Logger::Category::Warning);

	}
}

float TimeManager::SecondsSinceStart()
{
	if (instance != nullptr)
	{
		return std::chrono::duration<float, std::chrono::seconds::period>(std::chrono::high_resolution_clock::now() - instance->startTime).count();
	}
	else
	{
		Logger::Log(std::string("Calling TimeManager::SecondsSinceStart() before TimeManager::Initialize()"), Logger::Category::Warning);
	}

	return 0.0f;
}

float TimeManager::DeltaTime()
{
	if (instance != nullptr)
	{
		return instance->deltaTime;
	}
	else
	{
		Logger::Log(std::string("Calling TimeManager::DeltaTime() before TimeManager::Initialize()"), Logger::Category::Warning);
	}

	return 0.0f;
}

void TimeManager::RecordUpdateTime()
{
	if (instance != nullptr)
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		instance->deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - instance->lastUpdateTime).count();
		instance->lastUpdateTime = currentTime;

		instance->recentFrameTimes.push_back(instance->deltaTime);

		if (instance->recentFrameTimes.size() > 1000)
		{
			instance->recentFrameTimes.pop_front();
		}

		instance->frameID++;
	}
	else
	{
		Logger::Log(std::string("Calling TimeManager::RecordUpdateTime() before TimeManager::Initialize()"), Logger::Category::Warning);
	}
}

float TimeManager::GetAverageFPS()
{
	if (instance == nullptr)
	{
		return 0.0f;
	}

	float averageFrameTime = 0.0f;

	for (const auto& frameTime : instance->recentFrameTimes)
	{
		averageFrameTime += frameTime;
	}

	averageFrameTime /= instance->recentFrameTimes.size();

	return 1 / averageFrameTime;
}

unsigned long long TimeManager::GetFrameID()
{
	if (instance != nullptr)
	{
		return instance->frameID;
	};
}

TimeManager::TimeManager() :
	startTime(std::chrono::high_resolution_clock::now()),
	lastUpdateTime(startTime),
	deltaTime(0.0f)
{

}

TimeManager::~TimeManager()
{
	instance = nullptr;
}
