#ifndef GRAPHCISOBJECTTEXTUREDANIMATED_H
#define GRAPHICSOBJECTTEXTUREDANIMATED_H

#include "GraphicsObject3DComponent.h"

#include <string>

class Model;
class Texture;

class GraphicsObjectTexturedAnimated : public GraphicsObject3DComponent
{

public:

	GraphicsObjectTexturedAnimated() = default;

	GraphicsObjectTexturedAnimated(Model* const model, Texture* const texture);

	~GraphicsObjectTexturedAnimated();

	const glm::mat4* const GetAnimPoseArray() const;

	const std::vector<glm::mat4>& GetAnimInvBindPoseArray();

	void SetClip(const std::string& clipName);

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

	GraphicsObjectTexturedAnimated(const GraphicsObjectTexturedAnimated&) = delete;

	GraphicsObjectTexturedAnimated& operator=(const GraphicsObjectTexturedAnimated&) = delete;

	GraphicsObjectTexturedAnimated(GraphicsObjectTexturedAnimated&&) = delete;

	GraphicsObjectTexturedAnimated& operator=(GraphicsObjectTexturedAnimated&&) = delete;

	void Serialize() override;

	void Deserialize() override;

};

#endif // GRAPHICSOBJECTTEXTUREDANIMATED_H