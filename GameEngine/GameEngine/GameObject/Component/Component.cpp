#include "Component.h"

std::unordered_map<std::string, std::function<void(Component**)>> Component::newFunctions = std::unordered_map<std::string, std::function<void(Component**)>>();

std::function<void(Component**)> Component::GetConstructor(const std::string& name)
{
	if (newFunctions.find(name) != newFunctions.end())
	{
		return newFunctions[name];
	}

	return std::function<void(Component**)>();
}

Component::Component() :
	savedInts(),
	savedFloats(),
	savedVec3s(),
	savedVec2s(),
	savedVec4s(),
	savedMat4s(),
	savedStrings(),
	savedBools()
{
}

Component::~Component()
{
}
