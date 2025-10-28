#include "Renderer.h"

#include "../Utils/SingletonHelpers.h"
#include "Window/WindowManager.h"
#include "Camera/CameraManager.h"


#include <GLFW/glfw3.h>

Renderer* Renderer::instance = nullptr;

void GLFWErrorCallback(int error, const char* description)
{
	Logger::Log("Error: " + std::string(description), Logger::Category::Error);
}

bool Renderer::ShouldDraw()
{
	if (instance != nullptr)
	{
		return instance->draw;
	}
	return false;
}

void Renderer::Initialize()
{
	SingletonHelpers::InitializeSingleton<Renderer>(&instance, "Renderer");
}

void Renderer::Terminate()
{
	SingletonHelpers::TerminateSingleton<Renderer>(&instance, "Renderer");
}

void Renderer::CreateMainWindow(unsigned int width, unsigned int height, const std::string& name)
{
	WindowManager::CreateWindow(width, height, name);

	

}

void Renderer::Update()
{
	if (instance != nullptr)
	{
		WindowManager::Update();
		instance->shouldTerminate.store(WindowManager::ShouldTerminate());
	}
	else
	{
		Logger::Log("Calling Renderer::Update before Renderer::Initialize()");
	}
}

bool Renderer::ShouldTerminate()
{
	if (instance != nullptr)
	{
		return instance->shouldTerminate.load();
	}
	else
	{
		Logger::Log("Calling Renderer::ShouldTerminate() before Renderer::Initialize()");
		return false;
	}
}

void Renderer::InitializeGLFW()
{
	if (!glfwInit())
	{
		// Initialization Failed;
		Logger::Log("Failed to initialize GLFW", Logger::Category::Error);
	}
	else
	{
		glfwSetErrorCallback(GLFWErrorCallback);
		Logger::Log("Initialized GLFW", Logger::Category::Success);
	}
}

void Renderer::TerminateGLFW()
{
	glfwTerminate();
	Logger::Log("Terminated GLFW", Logger::Category::Success);
}

Renderer::Renderer() :
	draw(true)
{
	InitializeGLFW();
	WindowManager::Initialize();
	CameraManager::Initialize();
}

Renderer::~Renderer()
{
	CameraManager::Terminate();
	WindowManager::Terminate();
	TerminateGLFW();
}
