#include "EmptyComponent.h"

EmptyComponent::EmptyComponent()
{
}

EmptyComponent::~EmptyComponent()
{
}

bool EmptyComponent::GetBool(const std::string& name) const
{
	return savedBools.find(name)->second;
}

int EmptyComponent::GetInt(const std::string& name) const
{
	return savedInts.find(name)->second;
}

float EmptyComponent::GetFloat(const std::string& name) const
{
	return savedFloats.find(name)->second;
}

const std::string& EmptyComponent::GetString(const std::string& name) const
{
	return savedStrings.find(name)->second;
}

const glm::vec4& EmptyComponent::GetVec4(const std::string& name) const
{
	return savedVec4s.find(name)->second;
}

const glm::vec3& EmptyComponent::GetVec3(const std::string& name) const
{
	return savedVec3s.find(name)->second;
}

const glm::vec2& EmptyComponent::GetVec2(const std::string& name) const
{
	return savedVec2s.find(name)->second;
}

const glm::mat4 EmptyComponent::GetMat4(const std::string& name) const
{
	return savedMat4s.find(name)->second;
}

void EmptyComponent::SetBool(const std::string& name, bool val)
{
	savedBools[name] = val;
}

void EmptyComponent::SetInt(const std::string& name, int val)
{
	savedInts[name] = val;
}

void EmptyComponent::SetFloat(const std::string& name, float val)
{
	savedFloats[name] = val;
}

void EmptyComponent::SetString(const std::string& name, const std::string& val)
{
	savedStrings[name] = val;
}

void EmptyComponent::SetVec4(const std::string& name, const glm::vec4& val)
{
	savedVec4s[name] = val;
}

void EmptyComponent::SetVec3(const std::string& name, const glm::vec3& val)
{
	savedVec3s[name] = val;
}

void EmptyComponent::SetVec2(const std::string& name, const glm::vec2& val)
{
	savedVec2s[name] = val;
}

void EmptyComponent::SetMat4(const std::string& name, const glm::mat4& val)
{
	savedMat4s[name] = val;
}

void EmptyComponent::Serialize()
{
}

void EmptyComponent::Deserialize()
{
}

void EmptyComponent::Update()
{
}
