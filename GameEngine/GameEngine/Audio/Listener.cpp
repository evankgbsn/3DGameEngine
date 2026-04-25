#include "Listener.h"

Listener::Listener() :
    position(glm::vec3(0.0f)),
    rotation(glm::mat4(1.0f))
{
}

Listener::Listener(const glm::vec3& initialPos, const glm::mat4& initialRot) :
    position(initialPos),
    rotation(initialRot)
{
}

Listener::~Listener()
{
}

void Listener::SetPosition(const glm::vec3& pos)
{
    position = pos;
}

glm::vec3 Listener::GetPosition() const
{
    return position;
}

void Listener::SetRotation(const glm::mat4& rot)
{
    rotation = rot;
}

glm::mat4 Listener::GetRotation() const
{
    return rotation;
}

glm::vec3 Listener::GetForward() const
{
    return -glm::normalize(glm::vec3(rotation[2]));
}

glm::vec3 Listener::GetRight() const
{
    return glm::normalize(glm::vec3(rotation[0]));
}

glm::vec3 Listener::GetUp() const
{
    return glm::normalize(glm::vec3(rotation[1]));
}

IPLCoordinateSpace3 Listener::GetIPLCoordinateSystem() const
{
    IPLCoordinateSpace3 coords;
    glm::vec3 pos = GetPosition();
    glm::vec3 ahead = GetForward();
    glm::vec3 up = GetUp();
    glm::vec3 right = GetRight();

    coords.origin = { pos.x, pos.y, pos.z };
    coords.ahead = { ahead.x, ahead.y, ahead.z };
    coords.up = { up.x, up.y, up.z };
    coords.right = { right.x, right.y, right.z };

    return coords;
}
