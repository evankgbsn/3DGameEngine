import os

def create_networked_game_object(class_name):
    # Template strings incorporating NetworkObject inheritance
    header_template = """#ifndef {upper_name}_H
#define {upper_name}_H

#include "GameEngine/GameObject/GameObject.h"
#include "GameEngine/Networking/NetworkObject.h"

class {class_name} : public GameObject, public NetworkObject
{{
public:

	{class_name}();

	~{class_name}();

private:

	{class_name}(const {class_name}&) = delete;

	{class_name}& operator=(const {class_name}&) = delete;

	{class_name}({class_name}&&) = delete;

	{class_name}& operator=({class_name}&&) = delete;

	// --- GameObject Overrides ---
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

	// --- NetworkObject Overrides ---
	void OnServerSpawnConfirmation(const std::string& IP) override;

	void OnClientSpawnConfirmation() override;

	void OnSpawn() override;

	void OnDespawn() override;

	void OnDataReceived(const std::string& data) override;

}};

#endif // {upper_name}_H"""

    cpp_template = """#include "{class_name}.h"

{class_name}::{class_name}() :
	GameObject("{class_name}"),
	NetworkObject()
{{
	// Registers this class type for network instantiation
	RegisterNetworkObjectClassType<{class_name}>(this);
}}

{class_name}::~{class_name}()
{{
}}

// --- GameObject Implementation ---

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
}}

// --- NetworkObject Implementation ---

void {class_name}::OnServerSpawnConfirmation(const std::string& IP)
{{
	NetworkObject::OnServerSpawnConfirmation(IP);
}}

void {class_name}::OnClientSpawnConfirmation()
{{
}}

void {class_name}::OnSpawn()
{{
	// Base OnSpawn handles registering data receive functions
	NetworkObject::OnSpawn();
}}

void {class_name}::OnDespawn()
{{
	// Base OnDespawn handles deregistering data receive functions
	NetworkObject::OnDespawn();
}}

void {class_name}::OnDataReceived(const std::string& data)
{{
	// Base OnDataReceived handles packet ordering and callbacks
	NetworkObject::OnDataReceived(data);
}}"""

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

    print(f"Created Networked Class: {header_filename} and {cpp_filename}")

if __name__ == "__main__":
    name = input("Enter Networked GameObject name: ").strip()
    if name:
        create_networked_game_object(name)
