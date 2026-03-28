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

	bool isFading() const;

	float GetFadeToTime() const;

	const std::string& GetFadeToClipName() const;

	void RegisterAnimationStartCallback(const std::string& name, std::function<void(const std::string&)>* callback);

	void DeregisterAnimationStartCallback(const std::string& name);

	void RegisterAnimationStopCallback(const std::string& name, std::function<void(const std::string&)>* callback);

	void DeregisterAnimationStopCallback(const std::string& name);

private:


	GraphicsObjectColoredAnimated(const GraphicsObjectColoredAnimated&) = delete;

	GraphicsObjectColoredAnimated& operator=(const GraphicsObjectColoredAnimated&) = delete;

	GraphicsObjectColoredAnimated(GraphicsObjectColoredAnimated&&) = delete;

	GraphicsObjectColoredAnimated& operator=(GraphicsObjectColoredAnimated&&) = delete;

	void Serialize() override;

	void Deserialize() override;

};

#endif // GRAPHICSOBJECTCOLOREDANIMATED_H