#ifndef RENDERER_H
#define RENDERER_H

#include <glm/glm.hpp>

#include <string>
#include <atomic>

class Renderer
{
public:

	static bool ShouldDraw();

	static void SetShouldDraw(bool shouldDraw);

	static glm::vec2 GetPrimaryMonitorDimensions();

private:

	friend class SingletonHelpers;

	friend class Scene;

	friend class Engine;

	static void Initialize();

	static void Terminate();

	static void CreateMainWindow(unsigned int width, unsigned int height, const std::string& name);

	static void Update();

	static bool ShouldTerminate();

	void InitializeGLFW();

	void TerminateGLFW();

	Renderer();

	~Renderer();

	static Renderer* instance;

	std::atomic<bool> shouldTerminate;

	std::string mainWindowName;

	bool draw;
};

#endif // RENDERER_H