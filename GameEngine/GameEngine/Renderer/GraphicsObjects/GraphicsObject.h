#ifndef GRAPHICSOBJECT_H
#define GRAPHICSOBJECT_H

#include <vector>

class Model;

class GraphicsObject
{

public:

	const Model* const GetModel() const;

	bool IsDisabled() const;

protected:

	friend class GraphicsObjectManager;

	GraphicsObject(Model* const model);

	virtual ~GraphicsObject();

	virtual void Update() = 0;

	virtual void RenderToShadowMap();


	Model* model;

private:

	GraphicsObject(const GraphicsObject&) = delete;

	GraphicsObject& operator=(const GraphicsObject&) = delete;

	GraphicsObject(GraphicsObject&&) = delete;

	GraphicsObject& operator=(const GraphicsObject&&) = delete;

	unsigned int managerVectorIndex;

	unsigned int managerVectorDisableIndex;

	bool isDisabled;

};

#endif // GRAPHICSOBJECT_H