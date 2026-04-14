#ifndef TEXT_H
#define TEXT_H

#include <string>
#include <vector>

#include <glm/glm.hpp>

class GO3DGlyph;

class Text3D
{

public:

	Text3D(const std::string& string, const std::string& fontName, const glm::vec4& color = {1.0f, 1.0f, 1.0f, 1.0f}, const glm::vec3& position = { 0.0f, 0.0f, 0.0f }, const glm::vec2& scale = glm::vec2(1000.0f));

	void SetPosition(const glm::vec3& newPosition);

	glm::vec2 GetPosition() const;

	const std::string& GetString() const;

	const std::string& GetFontName() const;

	glm::vec3 GetColor() const;

	glm::vec2 GetScale() const;

	void SetScale(const glm::vec2& newScale);

	void Disable();

	void Enable();

	bool IsDisabled() const;

	void Append(const std::string& string);

	void PopBack();

	void PopFront();

	void SetText3D(const std::string& string);

	float GetCursorPosition() const;

	void SetRotation(const glm::mat4& newRotation);

	void SetRotation(float angle, const glm::vec3& axis);

	~Text3D();

private:

	Text3D() = delete;

	Text3D(const Text3D&) = delete;

	Text3D& operator=(const Text3D&) = delete;

	Text3D(Text3D&&) = delete;

	Text3D& operator=(Text3D&&) = delete;

	std::vector<GO3DGlyph*> glyphs;

	std::string text;

	glm::vec3 color;

	glm::vec3 position;

	std::string fontName;

	glm::vec2 scale;

	glm::mat4 rotation;

	bool disabled;

};

#endif // TEXT_H