#include "GraphicsObject.h"

#include "../Model/Model.h"

bool GraphicsObject::IsDisabled() const
{
	return isDisabled;
}

bool GraphicsObject::RenderShadow() const
{
	return renderShadow;
}

bool GraphicsObject::RenderReflection() const
{
	return renderReflection;
}

bool GraphicsObject::RenderGraphics() const
{
	return renderGraphics;
}

void GraphicsObject::SetRenderShadow(bool val)
{
	renderShadow = val;
}

void GraphicsObject::SetRenderReflection(bool val)
{
	renderReflection = val;
}

void GraphicsObject::SetRenderGraphics(bool val)
{
	renderGraphics = val;
}

const std::string& GraphicsObject::GetShaderName() const
{
	return shaderName;
}

GraphicsObject::GraphicsObject()
{
}

GraphicsObject::GraphicsObject(Model* const m, const std::string& shader) :
	model(m),
	isDisabled(false),
	shaderName(shader)
{
}

GraphicsObject::~GraphicsObject()
{
}

void GraphicsObject::RenderToShadowMap()
{

}

const Model* const GraphicsObject::GetModel() const
{
	return model;
}
