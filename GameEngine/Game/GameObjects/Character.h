#ifndef CHARACTER_H
#define CHARACTER_H

#include "GameEngine/GameObject/GameObject.h"

#include <glm/glm.hpp>

#include <functional>

class GraphicsObjectTexturedAnimatedLit;
class AnimatedColliderComponent;
class TextComponent;
class TerrainComponent;
class CameraComponent;

class Character : public GameObject
{

public:

	Character();

	~Character();

private:

	Character(const Character&) = delete;

	Character& operator=(const Character&) = delete;

	Character(Character&&) = delete;

	Character& operator=(Character&&) = delete;

	void Initialize() override;

	void Terminate() override;

	void GameUpdate() override;

	void EditorUpdate() override;

	void Load() override;

	void Unload() override;

	bool Hovered() const override;

	void SetPosition(const glm::vec3& newPos) override;

	void SetRotation(const glm::mat4& newRotation) override;

	void Start() override;

	void End() override;

	glm::vec3 GetPosition() const override;

	glm::mat4 GetRotation() const override;

	TerrainComponent* GetTerrain() const;

	void Serialize() override;

	void Deserialize() override;

	GraphicsObjectTexturedAnimatedLit* graphics;

	AnimatedColliderComponent* collider;

	std::function<void(int)>* toggleColliderVisibility;

	std::function<void(int)>* castLine;

	std::function<void(int)>* forward;
	std::function<void(int)>* backward;
	std::function<void(int)>* left;
	std::function<void(int)>* right;

	std::function<void(int)>* moveCamLeft;
	std::function<void(int)>* moveCamRight;
	std::function<void(int)>* moveCamUp;
	std::function<void(int)>* moveCamDown;

	std::function<void(int)>* updateCollisionVisuals;

	std::function<void(int)>* rotateCameraView;
	std::function<void(int)>* rotateCameraViewPress;
	std::function<void(int)>* rotateCameraViewRelease;
	std::function<void(double, double)>* zoomCamera;


	glm::vec3 cameraTarget;
	glm::vec3 camPosition;

	glm::vec3 targetPosition;

	float cameraDistance;

	TextComponent* text;

	CameraComponent* cam;
};

#endif // CHARACTER_H