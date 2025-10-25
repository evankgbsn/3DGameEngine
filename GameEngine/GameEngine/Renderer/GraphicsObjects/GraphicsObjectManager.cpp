#include "GraphicsObjectManager.h"

#include "GraphicsObject.h"
#include "GO3D.h"
#include "GOColored.h"
#include "GOColoredInstanced.h"
#include "GOTextured.h"
#include "GOColoredAnimated.h"
#include "GOTexturedAnimated.h"
#include "GOTexturedLit.h"
#include "GOTexturedAnimatedLit.h"
#include "GOLineColored.h"
#include "GOGlyph.h"
#include "GOTerrain.h"
#include "GOSprite.h"
#include "GOWater.h"
#include "../Font/FontManager.h"
#include "../../Utils/Logger.h"
#include "../Shader/ShaderManager.h"
#include "../Window/WindowManager.h"
#include "../Camera/CameraManager.h"
#include "../Camera/Camera.h"

GraphicsObjectManager* GraphicsObjectManager::instance = nullptr;

GOGlyph* glyph = nullptr;

void GraphicsObjectManager::Update()
{
	if (instance != nullptr)
	{
		GLint maxTextureSize;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);

		glViewport(0, 0, 8184, 8184);
		glBindFramebuffer(GL_FRAMEBUFFER, ShaderManager::GetShadowMapFramebuffer());
		glClear(GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_FRONT);

		for (GraphicsObject* graphicsObject : instance->graphicsObjects3D)
		{
			if (IsValid(graphicsObject))
			{
				graphicsObject->RenderToShadowMap();
			}
		}

		ShaderManager::BindReflectionFrameBuffer();
		glClear(GL_COLOR_BUFFER_BIT); 
		glClear(GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_BACK);
		ShaderManager::SetClipPlane({ 0.0f, 1.0f, 0.0f, 5.0f });

		Camera& activeCam = CameraManager::GetActiveCamera();
		glm::vec3 currentCamPos = activeCam.GetPosition();
		glm::vec3 currentTarget = activeCam.GetTarget();
		glm::vec3 currentCamForward = glm::normalize(currentTarget - currentCamPos);

		float distance = 2 * (currentCamPos.y - -5.0f);

		activeCam.SetPosition(currentCamPos + glm::vec3(0.0f, -distance, 0.0f));
		activeCam.SetTarget(activeCam.GetPosition() + glm::vec3(currentCamForward.x, -currentCamForward.y, currentCamForward.z));

		for (GraphicsObject* graphicsObject : instance->graphicsObjects3D)
		{
			if (IsValid(graphicsObject))
			{
				graphicsObject->Update();
			}
		}

		ShaderManager::UnbindCurrentFrameBuffer();

		activeCam.SetPosition(currentCamPos);
		activeCam.SetTarget(currentTarget);

		ShaderManager::BindRefractionFrameBuffer();
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_BACK);
		ShaderManager::SetClipPlane({ 0.0f, -1.0f, 0.0f, -5.0f });

		for (GraphicsObject* graphicsObject : instance->graphicsObjects3D)
		{
			if (IsValid(graphicsObject))
			{
				graphicsObject->Update();
			}
		}

		ShaderManager::UnbindCurrentFrameBuffer();


		glViewport(0, 0, WindowManager::GetWindow("Engine")->GetWidth(), WindowManager::GetWindow("Engine")->GetHeight());
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glCullFace(GL_BACK);
		ShaderManager::SetClipPlane({ 0.0f, -1.0f, 0.0f, 100000000.1f});
		
		for (GraphicsObject* graphicsObject : instance->graphicsObjects3D)
		{
			if (IsValid(graphicsObject))
			{
				graphicsObject->Update();
			}
		}

		for (GraphicsObject* graphicsObject : instance->graphicsObjectsWater)
		{
			if (IsValid(graphicsObject))
			{
				graphicsObject->Update();
			}
		}

		for (GraphicsObject* graphicsObject : instance->graphicsObjects2D)
		{
			if (IsValid(graphicsObject))
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

GOColoredInstanced* const GraphicsObjectManager::CreateGO3DColoredInstanced(Model* const model, const glm::vec4& initialColor, unsigned int instanceCount)
{
	GOColoredInstanced* result = nullptr;

	if (instance != nullptr)
	{
		instance->graphicsObjects3D.push_back(result = new GOColoredInstanced(model, initialColor, instanceCount));
		instance->graphicsObjects3D[instance->graphicsObjects3D.size() - 1]->managerVectorIndex = static_cast<unsigned int>(instance->graphicsObjects3D.size() - 1);
	}
	else
	{
		Logger::Log("Calling GraphicsObjectManager::CreateGO3DColoredInstanced() before GraphicsObjectManager::Initialize()", Logger::Category::Error);
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
		Logger::Log("Calling GraphicsObjectManager::CreateGO3DTexturedLit() before GraphicsObjectManager::Initialize()", Logger::Category::Error);
	}

	return result;
}

GOTerrain* const GraphicsObjectManager::CreateGOTerrain(Model* const model, const std::vector<GOLit::Material>& materials, const std::string& blendMap)
{
	GOTerrain* result = nullptr;

	if (instance != nullptr)
	{
		instance->graphicsObjects3D.push_back(result = new GOTerrain(model, materials, blendMap));
		instance->graphicsObjects3D[instance->graphicsObjects3D.size() - 1]->managerVectorIndex = static_cast<unsigned int>(instance->graphicsObjects3D.size() - 1);
	}
	else
	{
		Logger::Log("Calling GraphicsObjectManager::CreateGOTerrain() before GraphicsObjectManager::Initialize()", Logger::Category::Error);
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

GOLineColored* const GraphicsObjectManager::CreateGOLineColored(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color)
{
	GOLineColored* result = nullptr;

	if (instance != nullptr)
	{
		instance->graphicsObjects3D.push_back(result = new GOLineColored(start, end, color));
		instance->graphicsObjects3D[instance->graphicsObjects3D.size() - 1]->managerVectorIndex = static_cast<unsigned int>(instance->graphicsObjects3D.size() - 1);
	}
	else
	{
		Logger::Log("Calling GraphicsObjectManager::CreateGO3DColored() before GraphicsObjectManager::Initialize()", Logger::Category::Error);
	}

	return result;
}

GOGlyph* const GraphicsObjectManager::CreateGOGlyph(const Font::Glyph& glyph, const glm::vec4& color, const glm::vec2& position, const glm::vec2& scale)
{
	GOGlyph* result = nullptr;

	if (instance != nullptr)
	{
		instance->graphicsObjects2D.push_back(result = new GOGlyph(glyph, color, position, scale));
		instance->graphicsObjects2D[instance->graphicsObjects2D.size() - 1]->managerVectorIndex = static_cast<unsigned int>(instance->graphicsObjects2D.size() - 1);
	}
	else
	{
		Logger::Log("Calling GraphicsObjectManager::CreateGOGlyph() before GraphicsObjectManager::Initialize()", Logger::Category::Error);
	}

	return result;
}

GOSprite* const GraphicsObjectManager::CreateGOSprite(Model* const model2D, Texture* const imageTexture, const glm::vec2& position)
{
	GOSprite* result = nullptr;

	if (instance != nullptr)
	{
		instance->graphicsObjects2D.push_back(result = new GOSprite(model2D, imageTexture, position));
		instance->graphicsObjects2D[instance->graphicsObjects2D.size() - 1]->managerVectorIndex = static_cast<unsigned int>(instance->graphicsObjects2D.size() - 1);
	}
	else
	{
		Logger::Log("Calling GraphicsObjectManager::CreateGOSprite() before GraphicsObjectManager::Initialize()", Logger::Category::Error);
	}

	return result;
}

GOWater* const GraphicsObjectManager::CreateGOWater(Model* const model)
{
	GOWater* result = nullptr;

	if (instance != nullptr)
	{
		instance->graphicsObjectsWater.push_back(result = new GOWater(model));
		instance->graphicsObjectsWater[instance->graphicsObjectsWater.size() - 1]->managerVectorIndex = static_cast<unsigned int>(instance->graphicsObjectsWater.size() - 1);
	}
	else
	{
		Logger::Log("Calling GraphicsObjectManager::CreateGOWater() before GraphicsObjectManager::Initialize()", Logger::Category::Error);
	}

	return result;
}

void GraphicsObjectManager::Disable(GraphicsObject* const go)
{
	if (instance != nullptr)
	{
		GOWater* const water = dynamic_cast<GOWater* const>(go);

		if (go->managerVectorDisableIndex != UINT_MAX)
		{
			instance->disabledGraphicsObjects3D[go->managerVectorDisableIndex] = go;
		}
		else
		{
			instance->disabledGraphicsObjects3D.push_back(go);
			go->managerVectorDisableIndex = static_cast<unsigned int>(instance->disabledGraphicsObjects3D.size() - 1);
		}

		if (water != nullptr)
		{
			instance->graphicsObjectsWater[go->managerVectorIndex] = nullptr;
		}
		else
		{
			instance->graphicsObjects3D[go->managerVectorIndex] = nullptr;
		}
		
		go->isDisabled = true;
	}
}

void GraphicsObjectManager::Enable(GraphicsObject* const go)
{
	if (instance != nullptr)
	{
		GOWater* const water = dynamic_cast<GOWater* const>(go);

		if (water != nullptr)
		{
			instance->graphicsObjectsWater[go->managerVectorIndex] = go;

			if (go->isDisabled)
			{
				instance->disabledGraphicsObjects3D[go->managerVectorDisableIndex] = nullptr;
				go->isDisabled = false;
			}
		}
		else
		{
			instance->graphicsObjects3D[go->managerVectorIndex] = go;

			if (go->isDisabled)
			{
				instance->disabledGraphicsObjects3D[go->managerVectorDisableIndex] = nullptr;
				go->isDisabled = false;
			}
		}
	}
}

void GraphicsObjectManager::Delete(GraphicsObject* const go)
{
	if (instance != nullptr)
	{
		for (unsigned int i = 0; i < instance->graphicsObjects3D.size(); ++i)
		{
			if (instance->graphicsObjects3D[i] == go)
			{
				if (go != nullptr)
				{
					delete go;
					instance->graphicsObjects3D[i] = (GraphicsObject*)ULLONG_MAX;
					break;
				}
			}
		}

		for (unsigned int i = 0; i < instance->disabledGraphicsObjects3D.size(); ++i)
		{
			if (instance->disabledGraphicsObjects3D[i] == go)
			{
				if (go != nullptr)
				{
					delete go;
					instance->disabledGraphicsObjects3D[i] = (GraphicsObject*)ULLONG_MAX;
					break;
				}
			}
		}

		for (unsigned int i = 0; i < instance->graphicsObjects2D.size(); ++i)
		{
			if (instance->graphicsObjects2D[i] == go)
			{
				if (go != nullptr)
				{
					delete go;
					instance->graphicsObjects2D[i] = (GraphicsObject*)ULLONG_MAX;
					break;
				}
			}
		}

		for (unsigned int i = 0; i < instance->disabledGraphicsObjects2D.size(); ++i)
		{
			if (instance->disabledGraphicsObjects2D[i] == go)
			{
				if (go != nullptr)
				{
					delete go;
					instance->disabledGraphicsObjects2D[i] = (GraphicsObject*)ULLONG_MAX;
					break;
				}
			}
		}

		for (unsigned int i = 0; i < instance->graphicsObjectsWater.size(); ++i)
		{
			if (instance->graphicsObjectsWater[i] == go)
			{
				if (go != nullptr)
				{
					delete go;
					instance->graphicsObjectsWater[i] = (GraphicsObject*)ULLONG_MAX;
					break;
				}
			}
		}
	}
}

void GraphicsObjectManager::Enable(GOGlyph* const go)
{
	if (instance != nullptr)
	{
		instance->graphicsObjects2D[go->managerVectorIndex] = go;
		instance->disabledGraphicsObjects2D[go->managerVectorDisableIndex] = nullptr;
		go->isDisabled = false;
	}
}

void GraphicsObjectManager::Disable(GOGlyph* const go)
{
	if (instance != nullptr)
	{
		if (go->managerVectorDisableIndex != UINT_MAX)
		{
			instance->disabledGraphicsObjects2D[go->managerVectorDisableIndex] = go;
		}
		else
		{
			instance->disabledGraphicsObjects2D.push_back(go);
			go->managerVectorDisableIndex = static_cast<unsigned int>(instance->disabledGraphicsObjects2D.size() - 1);
		}

		instance->graphicsObjects2D[go->managerVectorIndex] = nullptr;
		go->isDisabled = true;
	}
}

void GraphicsObjectManager::Enable(GOSprite* const go)
{
	if (instance != nullptr)
	{
		instance->graphicsObjects2D[go->managerVectorIndex] = go;
		instance->disabledGraphicsObjects2D[go->managerVectorDisableIndex] = nullptr;
		go->isDisabled = false;
	}
}

void GraphicsObjectManager::Disable(GOSprite* const go)
{
	if (instance != nullptr)
	{
		if (go->managerVectorDisableIndex != UINT_MAX)
		{
			instance->disabledGraphicsObjects2D[go->managerVectorDisableIndex] = go;
		}
		else
		{
			instance->disabledGraphicsObjects2D.push_back(go);
			go->managerVectorDisableIndex = static_cast<unsigned int>(instance->disabledGraphicsObjects2D.size() - 1);
		}

		instance->graphicsObjects2D[go->managerVectorIndex] = nullptr;
		go->isDisabled = true;
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
		if (IsValid(graphicsObject))
		{
			delete graphicsObject;
		}
	}
}

bool GraphicsObjectManager::IsValid(GraphicsObject* graphicsObject)
{
	return graphicsObject != nullptr && graphicsObject != (GraphicsObject*)ULLONG_MAX;
}
