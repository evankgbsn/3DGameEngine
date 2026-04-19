import os

def create_game_object(class_name):
    # Template strings based on your Asteroid files
    header_template = """#ifndef {upper_name}_H
#define {upper_name}_H

#include "GameEngine/GameObject/GameObject.h"

class {class_name} : public GameObject
{{
public:

	{class_name}();

	~{class_name}();

private:

	{class_name}(const {class_name}&) = delete;

	{class_name}& operator=(const {class_name}&) = delete;

	{class_name}({class_name}&&) = delete;

	{class_name}& operator=({class_name}&&) = delete;

	void Initialize() override;

	void Terminate() override;

	void GameUpdate() override;

	void EditorUpdate() override;

	void Load() override;

	void Unload() override;

	bool Hovered() const override;

	glm::vec3 GetPosition() const override;

	void SetPosition(const glm::vec3& newPos) override;

	glm::mat4 GetRotation() const override;

	void SetRotation(const glm::mat4& newRot) override;

	glm::mat4 GetTransform() const override;

}};

#endif // {upper_name}_H"""

    cpp_template = """#include "{class_name}.h"

{class_name}::{class_name}() :
	GameObject("{class_name}")
{{
}}

{class_name}::~{class_name}()
{{
}}

void {class_name}::Initialize()
{{
}}

void {class_name}::Terminate()
{{
}}

void {class_name}::GameUpdate()
{{
}}

void {class_name}::EditorUpdate()
{{
}}

void {class_name}::Load()
{{
}}

void {class_name}::Unload()
{{
}}

bool {class_name}::Hovered() const
{{
	return false;
}}

glm::vec3 {class_name}::GetPosition() const
{{
	return glm::vec3();
}}

void {class_name}::SetPosition(const glm::vec3& newPos)
{{
}}

glm::mat4 {class_name}::GetRotation() const
{{
	return glm::mat4();
}}

void {class_name}::SetRotation(const glm::mat4& newRot)
{{
}}

glm::mat4 {class_name}::GetTransform() const
{{
	return glm::mat4();
}}"""

    # Prepare data
    data = {
        "class_name": class_name,
        "upper_name": class_name.upper()
    }

    # Write Header file
    header_filename = f"{class_name}.h"
    with open(header_filename, "w") as f:
        f.write(header_template.format(**data))

    # Write CPP file
    cpp_filename = f"{class_name}.cpp"
    with open(cpp_filename, "w") as f:
        f.write(cpp_template.format(**data))

    print(f"Successfully created {header_filename} and {cpp_filename}!")

# Example usage:
if __name__ == "__main__":
    new_class = input("Enter new GameObject class name: ").strip()
    if new_class:
        create_game_object(new_class)
