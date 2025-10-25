#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H


#include <glm/glm.hpp>

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
	
	static glm::vec4 GetClipPlane();

	static void SetClipPlane(const glm::vec4& newClipPlane);

private:

	friend class SingletonHelpers;

	friend class WindowManager;

	friend class GraphicsObjectManager;

	friend class GOTexturedLit;

	friend class GOTexturedAnimatedLit;

	friend class GOTerrain;

	friend class PointLight;

	friend class Renderer;

	friend class Shader;

	friend class Model;

	friend class TextureManager;

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

	static unsigned int GetRelfectionTexture();

	static unsigned int GetRelfractionTexture();

	static void BindReflectionFrameBuffer();

	static void BindRefractionFrameBuffer();

	static void UnbindCurrentFrameBuffer();

	void LoadShadersFromShaderDirectory();

	void CreateVAO();

	void CreateShadowMapFramebuffer();

	void DestroyShadowMapFramebuffer();

	void DestroyVAO();

	unsigned int CreateFrameBuffer();

	unsigned int CreateTextureAttatchment(unsigned int width, unsigned int height);

	unsigned int CreateDepthTextureAttatchment(unsigned int width, unsigned int height);

	unsigned int CreateDepthBufferAttatchment(unsigned int width, unsigned int height);

	void BindFrameBuffer(unsigned int frameBuffer, unsigned int width, unsigned int height);

	void InitializeWaterFrameBuffers();

	void CleanupWaterFrameBuffers();

	static void CreateWaterTexturesFromIDs();

	static ShaderManager* instance;

	static const std::string shaderDirectoryName;

	glm::vec4 clipPlane;

	std::vector<Shader*> shaders;

	unsigned int vertexArrayObject;

	GLDebug* debugInfo;

	unsigned int shadowMapFramebuffer;

	unsigned int shadowMapFramebufferDepthBufferTexture;

	unsigned int reflectionFrameBuffer;

	unsigned int reflectionTexture;

	unsigned int reflectionDepthBuffer;

	unsigned int refractionFrameBuffer;

	unsigned int refractionTexture;

	unsigned int refractionDepthTexture;
	
};

#endif // SHADERMANAGER_H