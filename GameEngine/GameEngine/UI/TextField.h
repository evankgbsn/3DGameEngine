#ifndef TEXTFIELD_H
#define TEXTFIELD_H

#include <glm/glm.hpp>

#include <string>
#include <functional>

class Text;

class TextField
{
public:

	TextField(const std::string& text = "", const std::string& font = "arial", const glm::vec2& position = { 0.5f, 0.5f }, const glm::vec2& size = glm::vec2(1.0f), const glm::vec4& color = glm::vec4(1.0f));

	~TextField();

	void SetPosition(const glm::vec2& newPosition);

	glm::vec2 GetPosition() const;

	const std::string& GetString() const;

	const std::string& GetFontName() const;

	glm::vec3 GetColor() const;

	glm::vec2 GetScale() const;

	void SetZ(float newZ);

	void Disable();

	void Enable();

	bool IsDisabled() const;

	void Append(const std::string& string);

	void PopBack();

	void PopFront();

	void SetText(const std::string& string);

private:

	TextField(const TextField&) = delete;

	TextField& operator=(const TextField&) = delete;

	TextField(TextField&&) = delete;

	TextField& operator=(TextField&&) = delete;

	Text* text;

	unsigned int ID;

	std::function<void(unsigned int, unsigned int)>* windowResizeCallback;

	unsigned int prevScreenWidth;

	unsigned int prevScreenHeight;

	bool stretch;

};

#endif // TEXTFILED_H