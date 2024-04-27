#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H



#include <vector>
#include <string>

class Shader;
class GLDebug;

class ShaderManager
{

public:


	static unsigned int GetShader(const std::string& name);

	static void StartShaderUsage(const std::string& name);

	static void EndShaderUsage(const std::string& name);

private:

	friend class SingletonHelpers;

	friend class WindowManager;

	friend class GraphicsObjectManager;

	friend class GOTexturedLit;

	friend class GOTexturedAnimatedLit;

	friend class Renderer;

	friend class Shader;

	friend class Model;

	static void Initialize();

	static void Terminate();

	static unsigned int GetVAO();

	static unsigned int GetShadowMapFramebuffer();

	static unsigned int GetShadowMapTexture();

	ShaderManager();

	~ShaderManager();

	ShaderManager(const ShaderManager&) = delete;

	ShaderManager& operator=(const ShaderManager&) = delete;

	ShaderManager(ShaderManager&&) = delete;

	ShaderManager& operator=(ShaderManager&&) = delete;

	void LoadShadersFromShaderDirectory();

	void CreateVAO();

	void CreateShadowMapFramebuffer();

	void DestroyShadowMapFramebuffer();

	void DestroyVAO();

	static ShaderManager* instance;

	static const std::string shaderDirectoryName;

	std::vector<Shader*> shaders;

	unsigned int vertexArrayObject;

	GLDebug* debugInfo;

	unsigned int shadowMapFramebuffer;

	unsigned int shadowMapFramebufferDepthBufferTexture;
	
};

#endif // SHADERMANAGER_H