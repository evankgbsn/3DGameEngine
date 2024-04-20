#include "GraphicsObject.h"

#include "../Model/Model.h"

bool GraphicsObject::IsDisabled() const
{
	return isDisabled;
}

GraphicsObject::GraphicsObject(Model* const m) :
	model(m),
	managerVectorIndex(UINT_MAX),
	managerVectorDisableIndex(UINT_MAX),
	isDisabled(false)
{
}

GraphicsObject::~GraphicsObject()
{
}
