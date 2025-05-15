#ifndef GRAPHICSOBJECTTEXTURED_H
#define GRAPHICSOBJECTTEXTURED_H

#include "GraphicsObject3DComponent.h"

#include <glm/glm.hpp>

class Model;
class Texture;
class GOTextured;

class GraphicsObjectTextured : public GraphicsObject3DComponent
{
public:

	GraphicsObjectTextured() = default;

	GraphicsObjectTextured(Model* const model, Texture* const texture);

	~GraphicsObjectTextured();

protected:

private:

	GraphicsObjectTextured(const GraphicsObjectTextured&) = delete;

	GraphicsObjectTextured& operator=(const GraphicsObjectTextured&) = delete;

	GraphicsObjectTextured(GraphicsObjectTextured&&) = delete;

	GraphicsObjectTextured& operator=(GraphicsObjectTextured&&) = delete;

	void Serialize() override;

	void Deserialize() override;

};

#endif // GRAPHICSOBJECTTEXTURED_H