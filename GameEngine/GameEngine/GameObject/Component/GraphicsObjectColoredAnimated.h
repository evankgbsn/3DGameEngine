#ifndef GRAPHICSOBJECTCOLOREDANIMATED_H
#define GRAPHICSOBJECTCOLOREDANIMATED_H

#include "GraphicsObject3DComponent.h"

#include <glm/glm.hpp>

class Model;

class GraphicsObjectColoredAnimated : public GraphicsObject3DComponent
{

public:

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

	const std::vector<char> Serialize() const override;

	void Deserialize(const std::vector<char>& data) override;

};

#endif // GRAPHICSOBJECTCOLOREDANIMATED_H