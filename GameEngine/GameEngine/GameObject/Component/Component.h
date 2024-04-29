#ifndef COMPONENT_H
#define COMPONENT_H

class Component
{

public:

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