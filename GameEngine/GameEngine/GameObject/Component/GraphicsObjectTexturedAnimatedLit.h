#ifndef GRAPHICSOBJECTTEXTUREDANIMATEDLIT_H
#define GRAPHICSOBJECTTEXTUREDANIMATEDLIT_H

#include "GraphicsObject3DComponent.h"

class Model;
class Texture;

class GraphicsObjectTexturedAnimatedLit : public GraphicsObject3DComponent
{

public:

	GraphicsObjectTexturedAnimatedLit(Model* const model, Texture* const diffuse, Texture* const specular);

	~GraphicsObjectTexturedAnimatedLit();

	void SetShine(float shine);

private:

	GraphicsObjectTexturedAnimatedLit(const GraphicsObjectTexturedAnimatedLit&) = delete;

	GraphicsObjectTexturedAnimatedLit& operator=(const GraphicsObjectTexturedAnimatedLit&) = delete;

	GraphicsObjectTexturedAnimatedLit(GraphicsObjectTexturedAnimatedLit&&) = delete;

	GraphicsObjectTexturedAnimatedLit& operator=(GraphicsObjectTexturedAnimatedLit&&) = delete;

};

#endif // GRAPHICSOBJECTTEXTUREDANIMATEDLIT_H