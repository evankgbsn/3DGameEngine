#ifndef JOINUI_H
#define JOINUI_H

#include "GameEngine/GameObject/GameObject.h"

#include <functional>

class Button;
class TextField;

class JoinUI : public GameObject
{
public:

	JoinUI();

	~JoinUI();

private:

	JoinUI(const JoinUI&) = delete;

	JoinUI& operator=(const JoinUI&) = delete;
	
	JoinUI(JoinUI&&) = delete;

	JoinUI& operator=(JoinUI&&) = delete;

	// Inherited via GameObject
	void Initialize() override;

	void Terminate() override;

	void GameUpdate() override;

	void EditorUpdate() override;

	void Load() override;

	void Unload() override;

	void Start() override;

	TextField* text;

	Button* button;

	std::function<void()>* buttonFunc;

};

#endif // JOINUI_H