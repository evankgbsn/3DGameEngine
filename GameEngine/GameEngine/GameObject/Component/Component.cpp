#include "Component.h"

std::unordered_map<std::string, std::function<void(Component**)>> Component::newFunctions = std::unordered_map<std::string, std::function<void(Component**)>>();

Component::Component()
{
}

Component::~Component()
{
}
