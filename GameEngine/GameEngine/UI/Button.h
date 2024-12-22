#ifndef BUTTON_H
#define BUTTON_H

#include <glm/glm.hpp>

#include <string>
#include <functional>

class GOSprite;
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

	GOSprite* sprite;

	Model* model;

	std::function<void()>* pressFunction;

	std::function<void()>* hoveredFunction;

	bool enabled;
};


#endif // BUTTON_H