#include "GraphicsObjectManager.h"

#include "GraphicsObject.h"
#include "GO3D.h"
#include "GOColored.h"
#include "GOColoredInstanced.h"
#include "GOTexturedLitInstanced.h"
#include "GOTextured.h"
#include "GOColoredAnimated.h"
#include "GOTexturedAnimated.h"
#include "GOTexturedLit.h"
#include "GOTexturedAnimatedLit.h"
#include "GOLineColored.h"
#include "GOGlyph.h"
#include "GO3DGlyph.h"
#include "GOTerrain.h"
#include "GOSprite.h"
#include "GOWater.h"
#include "../Font/FontManager.h"
#include "../../Utils/Logger.h"
#include "../Shader/ShaderManager.h"
#include "../Window/WindowManager.h"
#include "../Camera/CameraManager.h"
#include "../Camera/Camera.h"
#include "../Time/TimeManager.h"
#include "../Light/LightManager.h"

#include <algorithm>

GraphicsObjectManager* GraphicsObjectManager::instance = nullptr;

bool GraphicsObjectManager::terminated = true;

GOGlyph* glyph = nullptr;

void GraphicsObjectManager::Update()
{
	if (instance != nullptr)
	{
		std::unordered_map<std::string, std::vector<GraphicsObject*>> sortedObjects3D;
		std::unordered_map<std::string, std::vector<GraphicsObject*>> sortedObjects2D;

		for (const auto& graphicsObject : instance->graphicsObjects3D)
		{
			if (IsValid(graphicsObject))
			{
				sortedObjects3D[graphicsObject->GetShaderName()].push_back(graphicsObject);
			}
		}

		for (const auto& graphicsObject : instance->graphicsObjects2D)
		{
			if (IsValid(graphicsObject))
			{
				sortedObjects2D[graphicsObject->GetShaderName()].push_back(graphicsObject);
			}
		}

		std::sort(instance->graphicsObjects2DWorldSpace.begin(), instance->graphicsObjects2DWorldSpace.end(), [](GraphicsObject* go1, GraphicsObject* go2)
			{
				glm::vec3 camPos = CameraManager::GetActiveCamera().GetPosition();

				if (IsValid(go1) && IsValid(go2))
				{
					float distance1 = glm::length(camPos - static_cast<GO3D*>(go1)->GetTranslation());
					float distance2 = glm::length(camPos - static_cast<GO3D*>(go2)->GetTranslation());

					return distance1 > distance2;
				}

				return false;
			});

		GLint maxTextureSize;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);

		glViewport(0, 0, 8184, 8184);
		glBindFramebuffer(GL_FRAMEBUFFER, ShaderManager::GetShadowMapFramebuffer());
		glClear(GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_FRONT);

		instance->renderingShadows = true;
		for (GraphicsObject* graphicsObject : instance->graphicsObjects3D)
		{
			if (IsValid(graphicsObject) && graphicsObject->RenderShadow())
			{
				graphicsObject->RenderToShadowMap();
			}
		}
		instance->renderingShadows = false;

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

		instance->renderingReflections = true;
		for (const auto& shader : sortedObjects3D)
		{
			ShaderManager::StartShaderUsage(shader.first);

			for (GraphicsObject* graphicsObject : shader.second)
			{
				if (IsValid(graphicsObject) && graphicsObject->RenderReflection())
				{
					graphicsObject->Update();
				}
			}

			ShaderManager::EndShaderUsage(shader.first);
		}

		ShaderManager::UnbindCurrentFrameBuffer();

		activeCam.SetPosition(currentCamPos);
		activeCam.SetTarget(currentTarget);

		ShaderManager::BindRefractionFrameBuffer();
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_BACK);
		ShaderManager::SetClipPlane({ 0.0f, -1.0f, 0.0f, -5.0f });

		for (const auto& shader : sortedObjects3D)
		{
			ShaderManager::StartShaderUsage(shader.first);

			for (GraphicsObject* graphicsObject : shader.second)
			{
				if (IsValid(graphicsObject) && graphicsObject->RenderReflection())
				{
					graphicsObject->Update();
				}
			}

			ShaderManager::EndShaderUsage(shader.first);
		}
		instance->renderingReflections = false;

		ShaderManager::UnbindCurrentFrameBuffer();


		glViewport(0, 0, WindowManager::GetWindow("Engine")->GetWidth(), WindowManager::GetWindow("Engine")->GetHeight());
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glCullFace(GL_BACK);
		ShaderManager::SetClipPlane({ 0.0f, -1.0f, 0.0f, 100000000.1f});
		
		instance->renderingGraphics = true;
		for (const auto& shader : sortedObjects3D)
		{
			ShaderManager::StartShaderUsage(shader.first);

			for (GraphicsObject* graphicsObject : shader.second)
			{
				if (IsValid(graphicsObject) && graphicsObject->RenderGraphics())
				{
					graphicsObject->Update();
				}
			}

			ShaderManager::EndShaderUsage(shader.first);
		}

		ShaderManager::StartShaderUsage("Water");

		for (GraphicsObject* graphicsObject : instance->graphicsObjectsWater)
		{
			if (IsValid(graphicsObject) && graphicsObject->RenderGraphics())
			{
				graphicsObject->Update();
			}
		}

		ShaderManager::EndShaderUsage("Water");

		ShaderManager::StartShaderUsage("Font3D");

		for (const auto& graphicsObject : instance->graphicsObjects2DWorldSpace)
		{
			if (IsValid(graphicsObject) && graphicsObject->RenderGraphics())
			{
				graphicsObject->Update();
			}
		}

		ShaderManager::EndShaderUsage("Font3D");

		for (const auto& shader : sortedObjects2D)
		{
			ShaderManager::StartShaderUsage(shader.first);

			for (GraphicsObject* graphicsObject : shader.second)
			{
				if (IsValid(graphicsObject) && graphicsObject->RenderGraphics())
				{
					graphicsObject->Update();
				}
			}

			ShaderManager::EndShaderUsage(shader.first);
		}
		instance->renderingGraphics = false;

		for (GraphicsObject* graphicsObject : instance->graphicsObjects3D)
		{
			if (IsValid(graphicsObject))
			{
				GO3DAnimated* anim = dynamic_cast<GO3DAnimated*>(graphicsObject);
				if (anim != nullptr)
				{
					anim->ClearSetPose();
				}
			}
		}

		LightManager::ClearLightingUpdateBools();
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
		result = new GOColored(model, initialColor);
		instance->graphicsObjects3D.push_back(result);
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
	}
	else
	{
		Logger::Log("Calling GraphicsObjectManager::CreateGO3DColoredInstanced() before GraphicsObjectManager::Initialize()", Logger::Category::Error);
	}

	return result;
}

GOTexturedLitInstanced* const GraphicsObjectManager::CreateGO3DTexturedLitInstanced(Model* const model, Texture* const diffuse, Texture* const specular, Texture* const normal, unsigned int instanceCount)
{
	GOTexturedLitInstanced* result = nullptr;

	if (instance != nullptr)
	{
		instance->graphicsObjects3D.push_back(result = new GOTexturedLitInstanced(model, diffuse, specular, normal, instanceCount));
	}
	else
	{
		Logger::Log("Calling GraphicsObjectManager::CreateGO3DTexturedLitInstanced() before GraphicsObjectManager::Initialize()", Logger::Category::Error);
	}

	return result;
}

GOTextured* const GraphicsObjectManager::Create3DGOTextured(Model* const model, Texture* const texture)
{
	GOTextured* result = nullptr;

	if (instance != nullptr)
	{
		instance->graphicsObjects3D.push_back(result = new GOTextured(model, texture));
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
	}
	else
	{
		Logger::Log("Calling GraphicsObjectManager::CreateGO3DColored() before GraphicsObjectManager::Initialize()", Logger::Category::Error);
	}

	return result;
}

GOTexturedLit* const GraphicsObjectManager::CreateGO3DTexturedLit(Model* const model, const GOLit::Material& material)
{
	GOTexturedLit* result = nullptr;

	if (instance != nullptr)
	{
		instance->graphicsObjects3D.push_back(result = new GOTexturedLit(model, material));
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
	}
	else
	{
		Logger::Log("Calling GraphicsObjectManager::CreateGOTerrain() before GraphicsObjectManager::Initialize()", Logger::Category::Error);
	}

	return result;
}

GOTexturedAnimatedLit* const GraphicsObjectManager::CreateGO3DTexturedAnimatedLit(Model* const model, Texture* const diffuseMap, Texture* const specularMap, Texture* const normalMap)
{
	GOTexturedAnimatedLit* result = nullptr;

	if (instance != nullptr)
	{
		instance->graphicsObjects3D.push_back(result = new GOTexturedAnimatedLit(model, diffuseMap, specularMap, normalMap));
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
		result = new GOGlyph(glyph, color, position, scale);
		instance->graphicsObjects2D.push_back(result);
	}
	else
	{
		Logger::Log("Calling GraphicsObjectManager::CreateGOGlyph() before GraphicsObjectManager::Initialize()", Logger::Category::Error);
	}

	return result;
}

GO3DGlyph* const GraphicsObjectManager::CreateGO3DGlyph(const Font::Glyph& glyph, const glm::vec4& color, const glm::vec3& position, const glm::vec2& scale)
{
	GO3DGlyph* result = nullptr;

	if (instance != nullptr)
	{
		result = new GO3DGlyph(glyph, color, position, scale);
		instance->graphicsObjects2DWorldSpace.push_back(result);
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
	}
	else
	{
		Logger::Log("Calling GraphicsObjectManager::CreateGOWater() before GraphicsObjectManager::Initialize()", Logger::Category::Error);
	}

	return result;
}

void GraphicsObjectManager::Disable(GraphicsObject* const go)
{
	go->SetRenderGraphics(false);
	go->SetRenderReflection(false);
	go->SetRenderShadow(false);
	go->isDisabled = true;
}

void GraphicsObjectManager::Enable(GraphicsObject* const go)
{
	go->SetRenderGraphics(true);
	go->SetRenderReflection(true);
	go->SetRenderShadow(true);
	go->isDisabled = false;
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

		for (unsigned int i = 0; i < instance->graphicsObjects2DWorldSpace.size(); ++i)
		{
			if (instance->graphicsObjects2DWorldSpace[i] == go)
			{
				if (go != nullptr)
				{
					delete go;
					instance->graphicsObjects2DWorldSpace[i] = (GraphicsObject*)ULLONG_MAX;
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
	go->SetRenderGraphics(true);
	go->SetRenderReflection(true);
	go->SetRenderShadow(true);
	go->isDisabled = false;
}

void GraphicsObjectManager::Disable(GOGlyph* const go)
{
	go->SetRenderGraphics(false);
	go->SetRenderReflection(false);
	go->SetRenderShadow(false);
	go->isDisabled = true;
}

void GraphicsObjectManager::Enable(GOSprite* const go)
{
	go->SetRenderGraphics(true);
	go->SetRenderReflection(true);
	go->SetRenderShadow(true);
	go->isDisabled = false;
}

void GraphicsObjectManager::Disable(GOSprite* const go)
{
	go->SetRenderGraphics(false);
	go->SetRenderReflection(false);
	go->SetRenderShadow(false);
	go->isDisabled = true;
}

bool GraphicsObjectManager::RenderingShadows()
{
	if (instance != nullptr)
	{
		return instance->renderingShadows;
	}
	return false;
}

bool GraphicsObjectManager::RenderingReflections()
{
	if(instance != nullptr)
	{
		return instance->renderingReflections;
	}
	return false;
}

bool GraphicsObjectManager::RenderingGraphics()
{
	if(instance != nullptr)
	{
		return instance->renderingGraphics;
	}
	return false;
}

bool GraphicsObjectManager::Terminating()
{
	return terminated;
}

GraphicsObjectManager::GraphicsObjectManager() :
	graphicsObjects3D(std::vector<GraphicsObject*>())
{
	terminated = false;
}

GraphicsObjectManager::~GraphicsObjectManager()
{
	terminated = true;

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
