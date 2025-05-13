#ifndef GRAPHICSOBJECTTEXTUREDANIMATEDLIT_H
#define GRAPHICSOBJECTTEXTUREDANIMATEDLIT_H

#include "GraphicsObject3DComponent.h"

class Model;
class Texture;

class GraphicsObjectTexturedAnimatedLit : public GraphicsObject3DComponent
{

public:

	GraphicsObjectTexturedAnimatedLit() = default;

	GraphicsObjectTexturedAnimatedLit(Model* const model, Texture* const diffuse, Texture* const specular);

	~GraphicsObjectTexturedAnimatedLit();

	void SetShine(float shine);

	void SetClip(unsigned int clipNum);

	void SetSpeed(float speed);

private:

	GraphicsObjectTexturedAnimatedLit(const GraphicsObjectTexturedAnimatedLit&) = delete;

	GraphicsObjectTexturedAnimatedLit& operator=(const GraphicsObjectTexturedAnimatedLit&) = delete;

	GraphicsObjectTexturedAnimatedLit(GraphicsObjectTexturedAnimatedLit&&) = delete;

	GraphicsObjectTexturedAnimatedLit& operator=(GraphicsObjectTexturedAnimatedLit&&) = delete;

	const std::vector<char> Serialize() const override;

	void Deserialize(const std::vector<char>& data) override;

};

#endif // GRAPHICSOBJECTTEXTUREDANIMATEDLIT_H