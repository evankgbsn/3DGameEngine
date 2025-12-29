#ifndef GRAPHICSOBJECTCOLOREDANIMATED_H
#define GRAPHICSOBJECTCOLOREDANIMATED_H

#include "GraphicsObject3DComponent.h"

#include <glm/glm.hpp>

#include <vector>
#include <string>

class Model;

class GraphicsObjectColoredAnimated : public GraphicsObject3DComponent
{

public:

	GraphicsObjectColoredAnimated();

	GraphicsObjectColoredAnimated(Model* const model, const glm::vec4& color);

	~GraphicsObjectColoredAnimated();

	const glm::mat4* const GetAnimPoseArray() const;

	const std::vector<glm::mat4>& GetAnimInvBindPoseArray() const;

	void SetClip(const std::string& clipIndex);

	const std::string& GetClip() const;

	float GetSampleTime() const;

	void SetSampleTime(float sampleTime);

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