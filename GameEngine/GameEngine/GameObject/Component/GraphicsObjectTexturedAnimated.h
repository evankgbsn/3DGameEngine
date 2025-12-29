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

private:

	GraphicsObjectTexturedAnimated(const GraphicsObjectTexturedAnimated&) = delete;

	GraphicsObjectTexturedAnimated& operator=(const GraphicsObjectTexturedAnimated&) = delete;

	GraphicsObjectTexturedAnimated(GraphicsObjectTexturedAnimated&&) = delete;

	GraphicsObjectTexturedAnimated& operator=(GraphicsObjectTexturedAnimated&&) = delete;

	void Serialize() override;

	void Deserialize() override;

};

#endif // GRAPHICSOBJECTTEXTUREDANIMATED_H