#ifndef RENDERER_H
#define RENDERER_H

#include <string>
#include <atomic>

class Renderer
{
public:

	static bool ShouldDraw();

private:

	friend class SingletonHelpers;

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

	bool draw;
};

#endif // RENDERER_H