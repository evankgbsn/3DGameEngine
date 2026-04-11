#ifndef EMPTYCOMPONENT_H
#define EMPTYCOMPONENT_H

#include "Component.h"

class EmptyComponent : public Component
{
public:

	EmptyComponent();

	~EmptyComponent();

	bool GetBool(const std::string& name) const;

	int GetInt(const std::string& name) const;

	float GetFloat(const std::string& name) const;

	const std::string& GetString(const std::string& name) const;

	const glm::vec4& GetVec4(const std::string& name) const;

	const glm::vec3& GetVec3(const std::string& name) const;

	const glm::vec2& GetVec2(const std::string& name) const;

	const glm::mat4 GetMat4(const std::string& name) const;

	void SetBool(const std::string& name, bool val);

	void SetInt(const std::string& name, int val);

	void SetFloat(const std::string& name, float val);

	void SetString(const std::string& name, const std::string& val);

	void SetVec4(const std::string& name, const glm::vec4& val);

	void SetVec3(const std::string& name, const glm::vec3& val);

	void SetVec2(const std::string& name, const glm::vec2& val);

	void SetMat4(const std::string& name, const glm::mat4& val);

private:

	EmptyComponent(const EmptyComponent&) = delete;

	EmptyComponent& operator=(const EmptyComponent&) = delete;

	EmptyComponent(EmptyComponent&&) = delete;

	EmptyComponent& operator=(EmptyComponent&&) = delete;

	void Serialize() override;

	void Deserialize() override;

	void Update() override;
};

#endif // EMPTYCOMPONENET_H