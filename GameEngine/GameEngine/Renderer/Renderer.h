#ifndef RENDERER_H
#define RENDERER_H

#include <string>
#include <atomic>

class Renderer
{
public:

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
};

#endif // RENDERER_H