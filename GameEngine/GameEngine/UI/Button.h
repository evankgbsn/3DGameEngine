#ifndef BUTTON_H
#define BUTTON_H

#include <glm/glm.hpp>

#include <string>
#include <functional>

class Sprite;
class Texture;
class Model;

class Button
{
public:

	Button(const std::string& baseTextureName, const std::string& hoveredTextureName, const std::string& pressedTextureName, const std::string& model2DName, const glm::vec2& position, std::function<void()>* press);

	~Button();

	void Update();

	void Disable();

	void Enable();

	void SetScale(const glm::vec2& newScale);

	glm::vec2 GetScale() const;

private:

	Button() = delete;

	Button(const Button&) = delete;

	Button& operator=(const Button&) = delete;

	Button(Button&&) = delete;

	Button& operator=(Button&&) = delete;

	bool Hovered();

	bool Pressed();

	Texture* base;

	Texture* hovered;
	
	Texture* pressed;

	Sprite* sprite;

	Model* model;

	std::function<void()>* pressFunction;

	std::function<void()>* hoveredFunction;

	std::function<void(int)>* mousePress;

	glm::vec2 relativePosition;

	bool enabled;

	unsigned int ID;

	bool stretch;
};


#endif // BUTTON_H