#ifndef POSE_H
#define POSE_H

#include "../../Math/Transform.h"
#include "Clip.h"


#include <vector>

class Armature;

class Pose
{

public:

	static void Blend(Pose& out, Pose& a, Pose& b, float t, int root);

	static Pose MakeAdditivePose(Armature& armature, FastClip& clip);

	static void Add(Pose& out, Pose& in, Pose& addPose, Pose& additiveBasePose, int blendRoot);

	Pose();

	Pose(unsigned int numJoints);

	~Pose();

	Pose(const Pose&);

	Pose& operator=(const Pose&);

	Pose(Pose&&) = default;

	Pose& operator=(Pose&&) = default;

	bool operator==(const Pose& other);

	bool operator!=(const Pose& other);

	// This will retrieve the global transform of a joint.
	Math::Transform operator[](unsigned int index);

	Math::Transform GetGlobalTransform(unsigned int index) const;

	void Resize(unsigned int size);

	unsigned int Size() const;

	unsigned int GetParent(unsigned int index) const;

	void SetParent(unsigned int index, int parent);

	const Math::Transform& GetLocalTransform(unsigned int index) const;

	void SetLocalTransform(unsigned int index, const Math::Transform& transform);

	void GetJointMatrices(std::vector<glm::mat4>& outMatrices) const;

private:

	static bool IsInHierarchy(Pose& pose, unsigned int root, unsigned int search);

	std::vector<Math::Transform> joints;

	std::vector<int> parents;

};


#endif // POSE_H