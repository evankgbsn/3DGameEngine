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

	const std::vector<char> Serialize() const override;

	void Deserialize(const std::vector<char>& data) override;

};

#endif // GRAPHICSOBJECTTEXTURED_H