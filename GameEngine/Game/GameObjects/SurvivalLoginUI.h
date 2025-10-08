#ifndef SURVIVALLOGINUI_H
#define SURVIVALLOGINUI_H

#include "GameEngine/GameObject/GameObject.h"

#include <functional>

class InputField;

class SurvivalLoginUI : public GameObject
{
public:

	SurvivalLoginUI();

	~SurvivalLoginUI();

private:

	SurvivalLoginUI(const SurvivalLoginUI&) = delete;

	SurvivalLoginUI& operator=(const SurvivalLoginUI&) = delete;

	SurvivalLoginUI(SurvivalLoginUI&&) = delete;

	SurvivalLoginUI& operator=(SurvivalLoginUI&&) = delete;

	void Initialize() override;

	void Terminate() override;

	void GameUpdate() override;

	void EditorUpdate() override;

	void Load() override;

	void Unload() override;

	void Start() override;

	InputField* username;

	InputField* password;

	std::function<void()>* passwordOnEnter;

};

#endif // SURVIVALLOGINUI_H