#ifndef GRAPHICSOBJECTTEXTUREDLIT_H
#define GRAPHICSOBJECTTEXTUREDLIT_H

#include "GraphicsObject3DComponent.h"

class Model;
class Texture;

class GraphicsObjectTexturedLit : public GraphicsObject3DComponent
{

public:

	GraphicsObjectTexturedLit() = default;

	GraphicsObjectTexturedLit(Model* const model, Texture* const diffuse, Texture* const specular);

	~GraphicsObjectTexturedLit();

	void SetShine(float shine);

protected:

private:

	GraphicsObjectTexturedLit(const GraphicsObjectTexturedLit&) = delete;

	GraphicsObjectTexturedLit& operator=(const GraphicsObjectTexturedLit&) = delete;

	GraphicsObjectTexturedLit(GraphicsObjectTexturedLit&&) = delete;

	GraphicsObjectTexturedLit& operator=(GraphicsObjectTexturedLit&&) = delete;

	const std::vector<char> Serialize() const override;

	void Deserialize(const std::vector<char>& data) override;

};

#endif // GRAPHCISOBJECTTEXTUREDLIT_H