#ifndef GOTEXTUREDANIMATED_H
#define GOTEXTUREDANIMATED_H

#include "GO3DAnimated.h"

class Texture;
class Animation;

class GOTexturedAnimated : public GO3DAnimated
{

public:

	GOTexturedAnimated(Model* const model, Texture* const texture);

	~GOTexturedAnimated();

	void Update() override;

private:

	GOTexturedAnimated(const GOTexturedAnimated&) = delete;

	GOTexturedAnimated& operator=(const GOTexturedAnimated&) = delete;

	GOTexturedAnimated(GOTexturedAnimated&&) = delete;

	GOTexturedAnimated& operator=(GOTexturedAnimated&&) = delete;

	Texture* texture;

};

#endif // GOTEXTUREDANIMATED_H