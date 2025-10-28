#ifndef INPUTFIELD_H
#define INPUTFIELD_H

#include <string>
#include <functional>

#include <glm/glm.hpp>

class Sprite;
class Texture;
class Model;
class TextField;

class InputField
{

public:

	InputField(const std::string& baseTextureName, const std::string& hoveredTextureName, const std::string& pressedTextureName, const glm::vec2& dimensions, const glm::vec2& position, std::function<void()>* onEnter = nullptr, std::function<void()>* onChange = nullptr, bool editor = false);

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

	void SetZ(float newZ);

private:

	InputField() = delete;

	InputField(const InputField&) = delete;

	InputField& operator=(const InputField&) = delete;

	InputField(InputField&&) = delete;

	InputField& operator=(InputField&&) = delete;

	void SetupEnterCallback();

	void CleanupEnterCallback();

	static InputField* selectedInputField;

	Sprite* background;

	Sprite* cursor;

	Texture* base;

	Texture* hovered;

	Texture* pressed;

	TextField* text;

	std::function<void()>* onEnter;

	std::function<void()>* onChange;

	std::function<void()>* pressFunction;

	std::function<void()>* hoveredFunction;

	std::function<void(int)>* mousePress;

	std::function<void(int)>* enterCallback;

	std::function<void(int)>* keyPress;

	std::function<void(int)>* shiftPress;

	std::function<void(int)>* shiftRelease;

	std::function<void(int)>* backspacePressed;

	std::function<void(int)>* escPress;

	bool shiftPressed;

	bool enabled;

	bool selected;

	unsigned int maxCharacterLength;

	glm::vec2 relativePosition;

	unsigned int ID;

	bool isEditor;

	float lastBackspacePop;

	float lastBackspacePress;

	float lastPressTime;

};

#endif // INPUTFIELD_H