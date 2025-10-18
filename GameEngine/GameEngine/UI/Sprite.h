#ifndef SPRITE_H
#define SPRITE_H

#include <glm/glm.hpp>

#include <string>
#include <functional>

class GOSprite;

class Sprite
{
public:

	Sprite(const std::string& imageTexture, const glm::vec2& initialPosition, const glm::vec2& scale);

	~Sprite();

	glm::vec2 GetPosition() const;

	void SetPosition(const glm::vec2& newPosition);

	void Translate(const glm::vec2& translation);

	void SetTexture(const std::string& newTexture);

	const std::string& GetTexture() const;

	void SetScale(float x, float y);

	glm::vec2 GetScale() const;

	void Enable();

	void Disable();

	bool IsDisabled();

	void SetZ(float z);

	bool Hovered() const;

private:

	Sprite(const Sprite&) = delete;

	Sprite& operator=(const Sprite&) = delete;

	Sprite(Sprite&&) = delete;

	Sprite& operator=(Sprite&&) = delete;

	GOSprite* sprite;
	
	std::function<void(unsigned int, unsigned int)>* windowResizeCallback;

	unsigned int prevWindowWidth;

	unsigned int prevWindowHeight;

	bool stretch;

	unsigned int ID;

};

#endif // SPRITE