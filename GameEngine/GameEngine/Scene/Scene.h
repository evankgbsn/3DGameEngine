#ifndef SCENE_H
#define SCENE_H

class Scene
{
public:

	Scene();

	~Scene();

private:

	Scene(const Scene&) = delete;

	Scene& operator=(const Scene&) = delete;

	Scene(Scene&&) = delete;

	Scene& operator=(Scene&&) = delete;
};

#endif // SCENE_H