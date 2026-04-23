#include "Source.h"

Source::Source()
{
}

Source::~Source()
{
}

void Source::SetPosition(const glm::vec3& pos)
{
    position = pos;
}

glm::vec3 Source::GetPosition() const
{
    return position;
}

void Source::SetRotation(const glm::mat4& rot)
{
    rotation = rot;
}

glm::mat4 Source::GetRotation() const
{
    return rotation;
}

glm::vec3 Source::GetForward() const
{
    return glm::normalize(glm::vec3(rotation[2]));
}

glm::vec3 Source::GetRight() const
{
    return glm::normalize(glm::vec3(rotation[0]));
}

glm::vec3 Source::GetUp() const
{
    return glm::normalize(glm::vec3(rotation[1]));
}
