#include "GraphicsObjectManager.h"

#include "GraphicsObject.h"
#include "GO3D.h"
#include "GOColored.h"
#include "GOTextured.h"
#include "GOColoredAnimated.h"
#include "GOTexturedAnimated.h"
#include "GOTexturedLit.h"
#include "GOTexturedAnimatedLit.h"
#include "../../Utils/Logger.h"
#include "../Shader/ShaderManager.h"
#include "../Window/WindowManager.h"

GraphicsObjectManager* GraphicsObjectManager::instance = nullptr;

void GraphicsObjectManager::Update()
{
	if (instance != nullptr)
	{
		GLint maxTextureSize;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);

		glViewport(0, 0, maxTextureSize, maxTextureSize);
		glBindFramebuffer(GL_FRAMEBUFFER, ShaderManager::GetShadowMapFramebuffer());
		glClear(GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_FRONT);

		for (GraphicsObject* graphicsObject : instance->graphicsObjects3D)
		{
			if (graphicsObject != nullptr)
			{
				graphicsObject->RenderToShadowMap();
			}
		}

		glViewport(0, 0, WindowManager::GetWindow("Engine")->GetWidth(), WindowManager::GetWindow("Engine")->GetHeight());
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glCullFace(GL_BACK);
		
		for (GraphicsObject* graphicsObject : instance->graphicsObjects3D)
		{
			if (graphicsObject != nullptr)
			{
				graphicsObject->Update();
			}
		}
	}
	else
	{
		Logger::Log("Calling GraphicsObjectManager::Update() before GraphicsObjectManager::Initialize()", Logger::Category::Error);
	}
}

void GraphicsObjectManager::Initialize()
{
	if (instance == nullptr)
	{
		instance = new GraphicsObjectManager();
		Logger::Log("Initialized GraphicsObjectManager", Logger::Category::Success);
	}
	else
	{
		Logger::Log("Calling GraphicsObjectManager::Initialize() before GraphicsObjectManager::Terminate()", Logger::Category::Warning);
	}
}

void GraphicsObjectManager::Terminate()
{
	if (instance != nullptr)
	{
		delete instance;
		Logger::Log("Terminated GraphicsObjectManager", Logger::Category::Success);
	}
	else
	{
		Logger::Log("Calling GraphicsObjectManager::Terminate() before GraphicsObjectManager::Initialize()", Logger::Category::Warning);
	}
}

GOColored* const GraphicsObjectManager::CreateGO3DColored(Model* const model, const glm::vec4& initialColor)
{
	GOColored* result = nullptr;

	if (instance != nullptr)
	{
		instance->graphicsObjects3D.push_back(result = new GOColored(model, initialColor));
		instance->graphicsObjects3D[instance->graphicsObjects3D.size() - 1]->managerVectorIndex = static_cast<unsigned int>(instance->graphicsObjects3D.size() - 1);
	}
	else
	{
		Logger::Log("Calling GraphicsObjectManager::CreateGO3DColored() before GraphicsObjectManager::Initialize()", Logger::Category::Error);
	}

	return result;
}

GOTextured* const GraphicsObjectManager::Create3DGOTextured(Model* const model, Texture* const texture)
{
	GOTextured* result = nullptr;

	if (instance != nullptr)
	{
		instance->graphicsObjects3D.push_back(result = new GOTextured(model, texture));
		instance->graphicsObjects3D[instance->graphicsObjects3D.size() - 1]->managerVectorIndex = static_cast<unsigned int>(instance->graphicsObjects3D.size() - 1);
	}
	else
	{
		Logger::Log("Calling GraphicsObjectManager::CreateGO3DColored() before GraphicsObjectManager::Initialize()", Logger::Category::Error);
	}

	return result;
}

GOColoredAnimated* const GraphicsObjectManager::CreateGO3DColoredAnimated(Model* const model, const glm::vec4& initialColor)
{
	GOColoredAnimated* result = nullptr;

	if (instance != nullptr)
	{
		instance->graphicsObjects3D.push_back(result = new GOColoredAnimated(model, initialColor));
		instance->graphicsObjects3D[instance->graphicsObjects3D.size() - 1]->managerVectorIndex = static_cast<unsigned int>(instance->graphicsObjects3D.size() - 1);
	}
	else
	{
		Logger::Log("Calling GraphicsObjectManager::CreateGO3DColored() before GraphicsObjectManager::Initialize()", Logger::Category::Error);
	}

	return result;
}

GOTexturedAnimated* const GraphicsObjectManager::CreateGO3DTexturedAnimated(Model* const model, Texture* const texture)
{
	GOTexturedAnimated* result = nullptr;

	if (instance != nullptr)
	{
		instance->graphicsObjects3D.push_back(result = new GOTexturedAnimated(model, texture));
		instance->graphicsObjects3D[instance->graphicsObjects3D.size() - 1]->managerVectorIndex = static_cast<unsigned int>(instance->graphicsObjects3D.size() - 1);
	}
	else
	{
		Logger::Log("Calling GraphicsObjectManager::CreateGO3DColored() before GraphicsObjectManager::Initialize()", Logger::Category::Error);
	}

	return result;
}

GOTexturedLit* const GraphicsObjectManager::CreateGO3DTexturedLit(Model* const model, Texture* const diffuseMap, Texture* const specularMap)
{
	GOTexturedLit* result = nullptr;

	if (instance != nullptr)
	{
		instance->graphicsObjects3D.push_back(result = new GOTexturedLit(model, diffuseMap, specularMap));
		instance->graphicsObjects3D[instance->graphicsObjects3D.size() - 1]->managerVectorIndex = static_cast<unsigned int>(instance->graphicsObjects3D.size() - 1);
	}
	else
	{
		Logger::Log("Calling GraphicsObjectManager::CreateGO3DColored() before GraphicsObjectManager::Initialize()", Logger::Category::Error);
	}

	return result;
}

GOTexturedAnimatedLit* const GraphicsObjectManager::CreateGO3DTexturedAnimatedLit(Model* const model, Texture* const diffuseMap, Texture* const specularMap)
{
	GOTexturedAnimatedLit* result = nullptr;

	if (instance != nullptr)
	{
		instance->graphicsObjects3D.push_back(result = new GOTexturedAnimatedLit(model, diffuseMap, specularMap));
		instance->graphicsObjects3D[instance->graphicsObjects3D.size() - 1]->managerVectorIndex = static_cast<unsigned int>(instance->graphicsObjects3D.size() - 1);
	}
	else
	{
		Logger::Log("Calling GraphicsObjectManager::CreateGO3DColored() before GraphicsObjectManager::Initialize()", Logger::Category::Error);
	}

	return result;
}

void GraphicsObjectManager::Disable(GraphicsObject* const go)
{
	if (instance != nullptr)
	{
		if (go->managerVectorDisableIndex != UINT_MAX)
		{
			instance->disabledGraphicsObjects3D[go->managerVectorDisableIndex] = go;
		}
		else
		{
			instance->disabledGraphicsObjects3D.push_back(go);
			go->managerVectorDisableIndex = static_cast<unsigned int>(instance->disabledGraphicsObjects3D.size() - 1);
		}

		instance->graphicsObjects3D[go->managerVectorIndex] = nullptr;
		go->isDisabled = true;
	}
}

void GraphicsObjectManager::Enable(GraphicsObject* const go)
{
	if (instance != nullptr)
	{
		instance->graphicsObjects3D[go->managerVectorIndex] = go;
		instance->disabledGraphicsObjects3D[go->managerVectorDisableIndex] = nullptr;
		go->isDisabled = false;
	}
}

GraphicsObjectManager::GraphicsObjectManager() :
	graphicsObjects3D(std::vector<GraphicsObject*>())
{
}

GraphicsObjectManager::~GraphicsObjectManager()
{
	for (GraphicsObject* graphicsObject : graphicsObjects3D)
	{
		delete graphicsObject;
	}
}
