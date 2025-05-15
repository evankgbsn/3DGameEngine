#ifndef GRAPHICSOBJECTCOLORED_H
#define GRAPHICSOBJECTCOLORED_H

#include "GraphicsObject3DComponent.h"

#include <glm/glm.hpp>

class Model;

class GraphicsObjectColored : public GraphicsObject3DComponent
{

public:

	GraphicsObjectColored() = default;

	GraphicsObjectColored(Model* const model, const glm::vec4& color);

	~GraphicsObjectColored();

	void SetColor(const glm::vec4& newColor);

	const glm::vec4& GetColor() const;

protected:

private:

	GraphicsObjectColored(const GraphicsObjectColored&) = delete;

	GraphicsObjectColored& operator=(const GraphicsObjectColored&) = delete;

	GraphicsObjectColored(GraphicsObjectColored&&) = delete;

	GraphicsObjectColored& operator=(GraphicsObjectColored&&) = delete;

	void Serialize() override;

	void Deserialize() override;

};

#endif // GRAPHICSOBJECTTEXTURED_H