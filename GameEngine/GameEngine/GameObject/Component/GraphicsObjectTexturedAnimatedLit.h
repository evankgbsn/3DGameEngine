#ifndef GRAPHICSOBJECTTEXTUREDANIMATEDLIT_H
#define GRAPHICSOBJECTTEXTUREDANIMATEDLIT_H

#include "GraphicsObject3DComponent.h"

class Model;
class Texture;

class GraphicsObjectTexturedAnimatedLit : public GraphicsObject3DComponent
{

public:

	GraphicsObjectTexturedAnimatedLit();

	GraphicsObjectTexturedAnimatedLit(Model* const model, Texture* const diffuse, Texture* const specular);

	~GraphicsObjectTexturedAnimatedLit();

	void SetShine(float shine);

	void SetClip(unsigned int clipNum);

	void SetSpeed(float speed);

	glm::mat4 GetJointTransform(const std::string& jointName) const;

private:

	GraphicsObjectTexturedAnimatedLit(const GraphicsObjectTexturedAnimatedLit&) = delete;

	GraphicsObjectTexturedAnimatedLit& operator=(const GraphicsObjectTexturedAnimatedLit&) = delete;

	GraphicsObjectTexturedAnimatedLit(GraphicsObjectTexturedAnimatedLit&&) = delete;

	GraphicsObjectTexturedAnimatedLit& operator=(GraphicsObjectTexturedAnimatedLit&&) = delete;

	void Serialize() override;

	void Deserialize() override;

};

#endif // GRAPHICSOBJECTTEXTUREDANIMATEDLIT_H