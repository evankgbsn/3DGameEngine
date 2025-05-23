#ifndef GRAPHCISOBJECTTEXTUREDANIMATED_H
#define GRAPHICSOBJECTTEXTUREDANIMATED_H

#include "GraphicsObject3DComponent.h"

class Model;
class Texture;

class GraphicsObjectTexturedAnimated : public GraphicsObject3DComponent
{

public:

	GraphicsObjectTexturedAnimated() = default;

	GraphicsObjectTexturedAnimated(Model* const model, Texture* const texture);

	~GraphicsObjectTexturedAnimated();

	glm::mat4* GetAnimPoseArray();

	glm::mat4* GetAnimInvBindPoseArray();

	void SetClip(unsigned int clipIndex);

	unsigned int GetClip() const;

	unsigned int GetFrame() const;

	void SetFrame(unsigned int frameIndex);

	float GetSpeed() const;

	void SetSpeed(float speed);

private:

	GraphicsObjectTexturedAnimated(const GraphicsObjectTexturedAnimated&) = delete;

	GraphicsObjectTexturedAnimated& operator=(const GraphicsObjectTexturedAnimated&) = delete;

	GraphicsObjectTexturedAnimated(GraphicsObjectTexturedAnimated&&) = delete;

	GraphicsObjectTexturedAnimated& operator=(GraphicsObjectTexturedAnimated&&) = delete;

	void Serialize() override;

	void Deserialize() override;

};

#endif // GRAPHICSOBJECTTEXTUREDANIMATED_H