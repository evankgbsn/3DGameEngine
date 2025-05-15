#ifndef GRAPHICSOBJECTCOLOREDANIMATED_H
#define GRAPHICSOBJECTCOLOREDANIMATED_H

#include "GraphicsObject3DComponent.h"

#include <glm/glm.hpp>

class Model;

class GraphicsObjectColoredAnimated : public GraphicsObject3DComponent
{

public:

	GraphicsObjectColoredAnimated();

	GraphicsObjectColoredAnimated(Model* const model, const glm::vec4& color);

	~GraphicsObjectColoredAnimated();

	glm::mat4* GetAnimPoseArray();

	glm::mat4* GetAnimInvBindPoseArray();

	void SetClip(unsigned int clipIndex);

	unsigned int GetClip() const;

	unsigned int GetFrame() const;

	void SetFrame(unsigned int frameIndex);

	float GetSpeed() const;

	void SetSpeed(float speed);

private:


	GraphicsObjectColoredAnimated(const GraphicsObjectColoredAnimated&) = delete;

	GraphicsObjectColoredAnimated& operator=(const GraphicsObjectColoredAnimated&) = delete;

	GraphicsObjectColoredAnimated(GraphicsObjectColoredAnimated&&) = delete;

	GraphicsObjectColoredAnimated& operator=(GraphicsObjectColoredAnimated&&) = delete;

	void Serialize() override;

	void Deserialize() override;

};

#endif // GRAPHICSOBJECTCOLOREDANIMATED_H