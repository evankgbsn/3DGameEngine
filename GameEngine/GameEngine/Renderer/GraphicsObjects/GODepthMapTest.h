#ifndef DEPTHMAPTEST_H
#define DEPTHMAPTEST_H

#include "GO3D.h"

class GODepthMapTest : public GraphicsObject
{
public:

	GODepthMapTest(Model* const model);

	virtual ~GODepthMapTest();

	void Update() override;

protected:

private:

	GODepthMapTest() = delete;

	GODepthMapTest(const GODepthMapTest&) = delete;

	GODepthMapTest& operator=(const GODepthMapTest&) = delete;

	GODepthMapTest(GODepthMapTest&&) = delete;

	GODepthMapTest& operator=(GODepthMapTest&&) = delete;
};


#endif // DEPTHMAPTEST_H