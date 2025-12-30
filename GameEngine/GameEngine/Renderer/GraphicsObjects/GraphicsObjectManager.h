#ifndef GRAPHICSOBJECTMANAGER_H
#define GRAPHICSOBJECTMANAGER_H

#include "../Font/Font.h"

#include <glm/glm.hpp>

#include "GOLit.h"

#include <vector>

class Model;
class Texture;
class GraphicsObject;
class GO3D;
class GOColored;
class GOTextured;
class GOColoredAnimated;
class GOTexturedAnimated;
class GOTexturedLit;
class GOTexturedAnimatedLit;
class GOLineColored;
class GOGlyph;
class GOTerrain;
class GOColoredInstanced;
class GOSprite;
class GOWater;
class GOTexturedLitInstanced;


class GraphicsObjectManager
{
public:

	static GOColored* const CreateGO3DColored(Model* const model, const glm::vec4& initialColor);

	static GOColoredInstanced* const CreateGO3DColoredInstanced(Model* const model, const glm::vec4& initialColor, unsigned int instanceCount);

	static GOTexturedLitInstanced* const CreateGO3DTexturedLitInstanced(Model* const model, Texture* diffuse, Texture* specular, unsigned int instanceCount);

	static GOTextured* const Create3DGOTextured(Model* const model, Texture* const texture);

	static GOColoredAnimated* const CreateGO3DColoredAnimated(Model* const model, const glm::vec4& initialColor);

	static GOTexturedAnimated* const CreateGO3DTexturedAnimated(Model* const model, Texture* const texture);

	static GOTexturedLit* const CreateGO3DTexturedLit(Model* const model, Texture* const difuseMap, Texture* const specularMap);

	static GOTerrain* const CreateGOTerrain(Model* const model, const std::vector<GOLit::Material>& materials, const std::string& blendMap);

	static GOTexturedAnimatedLit* const CreateGO3DTexturedAnimatedLit(Model* const model, Texture* const difuseMap, Texture* const specularMap);

	static GOLineColored* const CreateGOLineColored(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color);

	static GOGlyph* const CreateGOGlyph(const Font::Glyph& glyph, const glm::vec4& color, const glm::vec2& position, const glm::vec2& scale);

	static GOSprite* const CreateGOSprite(Model* const model2D, Texture* const imageTexture, const glm::vec2& position);

	static GOWater* const CreateGOWater(Model* const model);

	static void Disable(GraphicsObject* const go);

	static void Enable(GraphicsObject* const go);

	static void Delete(GraphicsObject* const go);

	static void Enable(GOGlyph* const go);

	static void Disable(GOGlyph* const go);

	static void Enable(GOSprite* const go);

	static void Disable(GOSprite* const go);

	static bool RenderingShadows();

	static bool RenderingReflections();

	static bool RenderingGraphics();


private:

	friend class WindowManager;

	friend class Window;

	static void Update();

	static void Initialize();

	static void Terminate();

	GraphicsObjectManager();

	~GraphicsObjectManager();

	GraphicsObjectManager(const GraphicsObjectManager&) = delete;

	GraphicsObjectManager& operator=(const GraphicsObjectManager&) = delete;

	GraphicsObjectManager(GraphicsObjectManager&&) = delete;

	GraphicsObjectManager& operator=(GraphicsObjectManager&&) = delete;

	static bool IsValid(GraphicsObject* obj);

	static GraphicsObjectManager* instance;

	std::vector<GraphicsObject*> graphicsObjects3D;

	std::vector<GraphicsObject*> graphicsObjectsWater;

	std::vector<GraphicsObject*> graphicsObjects2D;

	bool renderingShadows = false;

	bool renderingGraphics = false;

	bool renderingReflections = false;
};

#endif // GRAPHICSOBJECTMANAGER_H
