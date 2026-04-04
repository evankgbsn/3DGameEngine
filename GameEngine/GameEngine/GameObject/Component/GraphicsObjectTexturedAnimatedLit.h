#ifndef GRAPHICSOBJECTTEXTUREDANIMATEDLIT_H
#define GRAPHICSOBJECTTEXTUREDANIMATEDLIT_H

#include "GraphicsObject3DComponent.h"

#include <glm/glm.hpp>

#include <vector>

class Model;
class Texture;

class GraphicsObjectTexturedAnimatedLit : public GraphicsObject3DComponent
{

public:

	GraphicsObjectTexturedAnimatedLit();

	GraphicsObjectTexturedAnimatedLit(Model* const model, const std::string& diffuse, const std::string& specular, const std::string& normal);

	~GraphicsObjectTexturedAnimatedLit();

	void SetShine(float shine);

	void SetClip(const std::string& clipName);

	void SetSpeed(float speed);

	glm::mat4 GetJointTransform(const std::string& jointName) const;

	const std::string& GetCurrentAnimation() const;

	void FadeAnimationTo(const std::string& animation, float time);

	void InitializeAdditiveAnimation(const std::string& animation);

	void SetAdditiveAnimationTime(const std::string& animation, float time);

	float GetAdditiveAnimationTime(const std::string& animation) const;

	bool isFading() const;

	float GetFadeToTime() const;

	const std::string& GetFadeToClipName() const;

	void RegisterAnimationStartCallback(const std::string& name, std::function<void(const std::string&)>* callback);

	void DeregisterAnimationStartCallback(const std::string& name);

	void RegisterAnimationStopCallback(const std::string& name, std::function<void(const std::string&)>* callback);

	void DeregisterAnimationStopCallback(const std::string& name);

private:

	GraphicsObjectTexturedAnimatedLit(const GraphicsObjectTexturedAnimatedLit&) = delete;

	GraphicsObjectTexturedAnimatedLit& operator=(const GraphicsObjectTexturedAnimatedLit&) = delete;

	GraphicsObjectTexturedAnimatedLit(GraphicsObjectTexturedAnimatedLit&&) = delete;

	GraphicsObjectTexturedAnimatedLit& operator=(GraphicsObjectTexturedAnimatedLit&&) = delete;

	void Serialize() override;

	void Deserialize() override;

};

#endif // GRAPHICSOBJECTTEXTUREDANIMATEDLIT_H