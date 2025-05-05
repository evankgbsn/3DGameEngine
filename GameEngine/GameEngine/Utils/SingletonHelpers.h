#ifndef SINGLETONHELPERS_H
#define SINGLETONHELPERS_H

#include "Logger.h"

#include <string>

class SingletonHelpers
{
public:

	template<typename T>
	static void InitializeSingleton(T** instance, const std::string& className)
	{
		if (*instance == nullptr)
		{
			*instance = new T();
			Logger::Log("Initialized " + className, Logger::Category::Success);
		}
		else
		{
			Logger::Log("Calling " + className + "::Initialzie() before " + className + "::Terminate()", Logger::Category::Error);
		}
	}

	template<typename T>
	static void TerminateSingleton(T** instance, const std::string& className)
	{
		if (*instance != nullptr)
		{
			delete* instance;
			*instance = nullptr;
			Logger::Log("Terminated " + className, Logger::Category::Success);
		}
		else
		{
			Logger::Log("Calling " + className + "::Terminate() before " + className + "::Initialize()", Logger::Category::Error);
		}
	}
};


#endif // SINGLETONHELPERS_H