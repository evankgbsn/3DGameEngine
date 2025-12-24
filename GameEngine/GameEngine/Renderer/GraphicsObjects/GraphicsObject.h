#ifndef GRAPHICSOBJECT_H
#define GRAPHICSOBJECT_H

#include <vector>

class Model;

class GraphicsObject
{

public:

	const Model* const GetModel() const;

	bool IsDisabled() const;

	bool RenderShadow() const;

	bool RenderReflection() const;

	bool RenderGraphics() const;

	void SetRenderShadow(bool val);

	void SetRenderReflection(bool val);

	void SetRenderGraphics(bool val);

protected:

	friend class GraphicsObjectManager;

	GraphicsObject();

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

	bool renderShadow = true;

	bool renderReflection = true;

	bool renderGraphics = true;

};

#endif // GRAPHICSOBJECT_H