#ifndef INPUTFIELD_H
#define INPUTFIELD_H

#include <string>
#include <functional>

#include <glm/glm.hpp>

class GOSprite;
class Texture;
class Model;
class Text;

class InputField
{

public:

	InputField(const std::string& baseTextureName, const std::string& hoveredTextureName, const std::string& pressedTextureName, const glm::vec2& dimensions, const glm::vec2& position, std::function<void()>* onEnter = nullptr, std::function<void()>* onChange = nullptr);

	~InputField();

	void Update();

	void Enable();

	void Disable();

	void SetOnEnter(std::function<void()>* onEnter);

	void SetOnChange(std::function<void()>* onChange);

	void Select();

	void Deselect();

	bool Hovered();

	bool Pressed();

	bool Selected();

	void SetMaxCharacterLength(unsigned int length);

	const std::string& GetText() const;

private:

	InputField() = delete;

	InputField(const InputField&) = delete;

	InputField& operator=(const InputField&) = delete;

	InputField(InputField&&) = delete;

	InputField& operator=(InputField&&) = delete;

	void SetupEnterCallback();

	void CleanupEnterCallback();

	GOSprite* background;

	Texture* base;

	Texture* hovered;

	Texture* pressed;

	Model* model;

	Text* text;

	std::function<void()>* onEnter;

	std::function<void()>* onChange;

	std::function<void()>* pressFunction;

	std::function<void()>* hoveredFunction;

	std::function<void(int)>* mousePress;

	std::function<void(int)>* enterCallback;

	std::function<void(int)>* keyPress;

	bool enabled;

	bool selected;

	unsigned int maxCharacterLength;

	glm::vec2 relativePosition;

	std::function<void(unsigned int, unsigned int)>* windowResizeCallback;

	unsigned int ID;

};

#endif // INPUTFIELD_H