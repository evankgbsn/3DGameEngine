#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>
#include <vector>

class Component
{

public:

	virtual const std::vector<char> Serialize() const = 0;

	virtual void Deserialize(const std::vector<char>& data) = 0;

protected:

	Component();

	~Component();

private:

	friend class GameObject;

	Component(const Component&) = delete;

	Component& operator=(const Component&) = delete;

	Component(Component&&) = delete;

	Component& operator=(Component&&) = delete;

	virtual void Update() = 0;

};

#endif // COMPONENT_H