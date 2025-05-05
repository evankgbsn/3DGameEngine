#include "ShaderManager.h"

#include "../../Utils/SingletonHelpers.h"
#include "Shader.h"
#include "../../Utils/OpenGLDebug.h"
#include "../Camera/Camera.h"
#include "../Camera/CameraManager.h"
#include "../Window/WindowManager.h"

#include <GLFW/glfw3.h>

#include <filesystem>
#include <unordered_map>

ShaderManager* ShaderManager::instance = nullptr;

const std::string ShaderManager::shaderDirectoryName = std::string("Assets/GLSL/");

unsigned int ShaderManager::GetShader(const std::string& name)
{
	if (instance != nullptr)
	{
		for (const Shader* const shader : instance->shaders)
		{
			if (shader->GetName() == name)
			{
				return shader->GetId();
			}
		}

		return instance->shaders[0]->GetId();
	}
	else
	{
		return UINT_MAX;
		Logger::Log("Calling ShaderManager::GetShader() before ShaderManager::Initialize()", Logger::Category::Error);
	}
}
	

void ShaderManager::StartShaderUsage(const std::string& name)
{
	if (instance != nullptr)
	{
		for (const Shader* const shader : instance->shaders)
		{
			if (shader->GetName() == name)
			{
				glUseProgram(shader->GetId());
				break;
			}
		}
	}
	else
	{
		Logger::Log("Calling ShaderManager::StartShaderUsage() before ShaderManager::Initialize()", Logger::Category::Error);
	}
}

void ShaderManager::EndShaderUsage(const std::string& name)
{
	if (instance != nullptr)
	{
		for (const Shader* const shader : instance->shaders)
		{
			if (shader->GetName() == name)
			{
				
			}
		}
	}
	else
	{
		Logger::Log("Calling ShaderManager::StartShaderUsage() before ShaderManager::Initialize()", Logger::Category::Error);
	}
}

void ShaderManager::Initialize()
{
	SingletonHelpers::InitializeSingleton(&instance, "ShaderManager");
}

void ShaderManager::Terminate()
{
	SingletonHelpers::TerminateSingleton(&instance, "ShaderManager");
}

ShaderManager::ShaderManager()
{
	debugInfo = new GLDebug();
	LoadShadersFromShaderDirectory();
	CreateVAO();
	CreateShadowMapFramebuffer();
}

ShaderManager::~ShaderManager()
{
	for (auto& shader : shaders)
	{
		delete shader;
	}

	DestroyShadowMapFramebuffer();
	DestroyVAO();
	delete debugInfo;
}

void ShaderManager::LoadShadersFromShaderDirectory()
{
	std::filesystem::path shaderDirectory = shaderDirectoryName;
	bool shaderFilePathExists = std::filesystem::exists(shaderDirectory);

	if (shaderFilePathExists)
	{
		std::unordered_map<std::string, std::vector<std::filesystem::directory_entry>> shaderFiles;

		for (auto& dirEntry : std::filesystem::directory_iterator(shaderDirectory))
		{
			if (dirEntry.is_regular_file())
			{
				if (shaderFiles.find(dirEntry.path().stem().string()) == shaderFiles.end())
				{
					shaderFiles[dirEntry.path().stem().string()].resize(5);
				}

				if (dirEntry.path().has_extension())
				{
					static const std::string vert(".vert");
					static const std::string frag(".frag");
					static const std::string geo(".geo");
					static const std::string tessE(".tesse");
					static const std::string tessC(".tessc");

					if (dirEntry.path().extension() == vert)
					{
						shaderFiles[dirEntry.path().stem().string()][0] = dirEntry;
					}
					else if (dirEntry.path().extension() == frag)
					{
						shaderFiles[dirEntry.path().stem().string()][1] = dirEntry;
					}
					else if (dirEntry.path().extension() == tessE)
					{
						shaderFiles[dirEntry.path().filename().string()][2] = dirEntry;
					}
					else if (dirEntry.path().extension() == tessC)
					{
						shaderFiles[dirEntry.path().stem().string()][3] = dirEntry;
					}
					else if (dirEntry.path().extension() == geo)
					{
						shaderFiles[dirEntry.path().stem().string()][4] = dirEntry;
					}
				}
			}
		}

		for (auto& shader : shaderFiles)
		{
			shaders.push_back(new Shader(shader.first, shader.second[0], shader.second[1], shader.second[2], shader.second[3], shader.second[4]));
		}
	}
}

void ShaderManager::CreateVAO()
{
	glCreateVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);

	glVertexArrayAttribBinding(vertexArrayObject, 3, 0);
	glVertexArrayAttribFormat(vertexArrayObject, 3, 4, GL_FLOAT, GL_FALSE, 0);
	
	glVertexArrayAttribBinding(vertexArrayObject, 4, 0);
	glVertexArrayAttribIFormat(vertexArrayObject, 4, 4, GL_INT, 16);

	glVertexArrayAttribBinding(vertexArrayObject, 1, 0);
	glVertexArrayAttribFormat(vertexArrayObject, 1, 3, GL_FLOAT, GL_FALSE, 32);

	glVertexArrayAttribBinding(vertexArrayObject, 0, 0);
	glVertexArrayAttribFormat(vertexArrayObject, 0, 3, GL_FLOAT, GL_FALSE, 44);

	glVertexArrayAttribBinding(vertexArrayObject, 2, 0);
	glVertexArrayAttribFormat(vertexArrayObject, 2, 2, GL_FLOAT, GL_FALSE, 56);


	glEnableVertexArrayAttrib(vertexArrayObject, 0);
	glEnableVertexArrayAttrib(vertexArrayObject, 1);
	glEnableVertexArrayAttrib(vertexArrayObject, 2);
	glEnableVertexArrayAttrib(vertexArrayObject, 3);
	glEnableVertexArrayAttrib(vertexArrayObject, 4);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_FRAMEBUFFER_SRGB);
	
}

void ShaderManager::CreateShadowMapFramebuffer()
{
	glCreateFramebuffers(1, &shadowMapFramebuffer);
	glCreateTextures(GL_TEXTURE_2D, 1, &shadowMapFramebufferDepthBufferTexture);
	glBindTexture(GL_TEXTURE_2D, shadowMapFramebufferDepthBufferTexture);

	GLint maxTextureSize;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 8184, 8184, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);


	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFramebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMapFramebufferDepthBufferTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	Camera& depthOrtho = CameraManager::CreateCamera(Camera::Type::ORTHOGRAPHIC, "ShadowMapDepth", WindowManager::GetWindow("Engine"));
	depthOrtho.SetNear(1.0f);
	depthOrtho.SetFar(7.5f);
	depthOrtho.SetPosition({ 50.f, 50.f, -50.f });
	depthOrtho.SetTarget(glm::vec3(0.0f, 0.0f, 0.0f));
}

void ShaderManager::DestroyShadowMapFramebuffer()
{
	glDeleteTextures(1, &shadowMapFramebufferDepthBufferTexture);
	glDeleteFramebuffers(1, &shadowMapFramebuffer);
}

void ShaderManager::DestroyVAO()
{
	glDeleteVertexArrays(1, &vertexArrayObject);
}

unsigned int ShaderManager::GetVAO()
{
	return instance->vertexArrayObject;
}

unsigned int ShaderManager::GetShadowMapFramebuffer()
{
	return instance->shadowMapFramebuffer;
}

unsigned int ShaderManager::GetShadowMapTexture()
{
	return instance->shadowMapFramebufferDepthBufferTexture;
}
