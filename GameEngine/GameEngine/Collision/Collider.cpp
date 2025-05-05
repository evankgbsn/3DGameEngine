#include "Collider.h"

bool Collider::IsVisible() const
{
	return visible;
}

Collider::Collider() :
	visible(true)
{
}

Collider::~Collider()
{
}
