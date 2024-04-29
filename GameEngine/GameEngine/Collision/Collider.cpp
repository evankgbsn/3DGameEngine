#include "Collider.h"

bool Collider::Visible() const
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
