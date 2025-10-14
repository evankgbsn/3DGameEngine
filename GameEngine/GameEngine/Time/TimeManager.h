#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H

#include <chrono>
#include <list>

class TimeManager
{

public:

	static void Initialize();

	static void Terminate();

	static float SecondsSinceStart();

	static float DeltaTime();

	static void RecordUpdateTime();

	static float GetAverageFPS();

private:

	TimeManager();

	~TimeManager();

	TimeManager(const TimeManager&) = delete;

	TimeManager& operator=(const TimeManager&) = delete;

	TimeManager(TimeManager&&) = delete;

	TimeManager& operator=(TimeManager&&) = delete;

	static TimeManager* instance;

	std::chrono::high_resolution_clock::time_point startTime;

	std::chrono::high_resolution_clock::time_point lastUpdateTime;

	float deltaTime;

	std::list<float> recentFrameTimes;

};



#endif // TIMEMANAGER_H