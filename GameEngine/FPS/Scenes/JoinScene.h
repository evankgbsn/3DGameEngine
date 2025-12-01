#ifndef JOINSCENE_H
#define JOINSCENE_H

#include "GameEngine/Scene/Scene.h"

class JoinUI;

class JoinScene : public Scene
{
public:

	JoinScene();

	~JoinScene();

private:

	JoinScene(const JoinScene&) = delete;

	JoinScene& operator=(const JoinScene&) = delete;

	JoinScene(JoinScene&&) = delete;

	JoinScene& operator=(JoinScene&&) = delete;

	void Initialize() override;

	void Terminate() override;

	void Load() override;

	void Unload() override;

	JoinUI* ui;
};

#endif JOINSCENE_H

