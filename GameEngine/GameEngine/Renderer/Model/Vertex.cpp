#include "Vertex.h"

Vertex::Vertex() :
	weights(vec4(0.0f)),
	influences(ivec4(0)),
	position(vec3(0.0f)),
	normal(vec3(0.0f)),
	uv(vec2(0.0f))
{

}

Vertex::Vertex(const vec3& p, const vec3& n, const vec2& uvCoord) :
	weights(vec4(0.0f)),
	influences(ivec4(0)),
	position(p),
	normal(n),
	uv(uvCoord)
{

}

Vertex::~Vertex()
{

}

bool Vertex::operator==(const Vertex& other) const
{
	return position == other.position && normal == other.normal && uv == other.uv && weights == other.weights && influences == other.influences;
}

bool Vertex::operator!=(const Vertex& other) const
{
	return !(*this == other);
}

vec4& Vertex::GetWeights()
{
	return weights;
}

ivec4& Vertex::GetInfluences()
{
	return influences;
}

vec3& Vertex::GetPosition()
{
	return position;
}

vec3& Vertex::GetNormal()
{
	return normal;
}

vec2& Vertex::GetUV()
{
	return uv;
}

const vec4& Vertex::GetWeights() const
{
	return weights;
}

const ivec4& Vertex::GetInfluences() const
{
	return influences;
}

const vec3& Vertex::GetPosition() const
{
	return position;
}

const vec3& Vertex::GetNormal() const
{
	return normal;
}

const vec2& Vertex::GetUV() const
{
	return uv;
}
