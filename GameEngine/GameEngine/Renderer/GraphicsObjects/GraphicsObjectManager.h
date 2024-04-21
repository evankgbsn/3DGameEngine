#ifndef GRAPHICSOBJECTMANAGER_H
#define GRAPHICSOBJECTMANAGER_H

#include <glm/glm.hpp>

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

class GraphicsObjectManager
{
public:

	static GOColored* const CreateGO3DColored(Model* const model, const glm::vec4& initialColor);

	static GOTextured* const Create3DGOTextured(Model* const model, Texture* const texture);

	static GOColoredAnimated* const CreateGO3DColoredAnimated(Model* const model, const glm::vec4& initialColor);

	static GOTexturedAnimated* const CreateGO3DTexturedAnimated(Model* const model, Texture* const texture);

	static GOTexturedLit* const CreateGO3DTexturedLit(Model* const model, Texture* const texture);

	static GOTexturedAnimatedLit* const CreateGO3DTexturedAnimatedLit(Model* const model, Texture* const texture);

	static void Disable(GraphicsObject* const go);

	static void Enable(GraphicsObject* const go);

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

	static GraphicsObjectManager* instance;

	std::vector<GraphicsObject*> graphicsObjects3D;

	std::vector<GraphicsObject*> disabledGraphicsObjects3D;

};

#endif // GRAPHICSOBJECTMANAGER_H
