#include "TransformTrack.h"

template TTransformTrack<VectorTrack, QuaternionTrack>;
template TTransformTrack<VectorFastTrack, QuaternionFastTrack>;

template<typename VTRACK, typename QTRACK>
TTransformTrack<VTRACK, QTRACK>::TTransformTrack() :
	id(),
	position(),
	rotation(),
	scale()
{
}


template<typename VTRACK, typename QTRACK>
TTransformTrack<VTRACK, QTRACK>::~TTransformTrack()
{
}

template<typename VTRACK, typename QTRACK>
unsigned int TTransformTrack<VTRACK, QTRACK>::GetId() const
{
	return id;
}

template<typename VTRACK, typename QTRACK>
void TTransformTrack<VTRACK, QTRACK>::SetId(const unsigned int& newId)
{
	id = newId;
}

template<typename VTRACK, typename QTRACK>
VTRACK& TTransformTrack<VTRACK, QTRACK>::GetPositionTrack()
{
	return position;
}

template<typename VTRACK, typename QTRACK>
QTRACK& TTransformTrack<VTRACK, QTRACK>::GetRotationTrack()
{
	return rotation;
}

template<typename VTRACK, typename QTRACK>
VTRACK& TTransformTrack<VTRACK, QTRACK>::GetScaleTrack()
{
	return scale;
}

template<typename VTRACK, typename QTRACK>
float TTransformTrack<VTRACK, QTRACK>::GetStartTime() const
{
	float result = 0.0f;
	bool isSet = false;

	if (position.Size() > 1)
	{
		result = position.GetStartTime();
		isSet = true;
	}

	if (rotation.Size() > 1)
	{
		float rotationStart = rotation.GetStartTime();
		if (rotationStart < result || !isSet)
		{
			result = rotationStart;
			isSet = true;
		}
	}

	if (scale.Size() > 1)
	{
		float scaleStart = scale.GetStartTime();
		if (scaleStart < result || !isSet)
		{
			result = scaleStart;
			isSet = true;
		}
	}

	return result;
}

template<typename VTRACK, typename QTRACK>
float TTransformTrack<VTRACK, QTRACK>::GetEndTime() const
{
	float result = 0.0f;
	bool isSet = false;

	if (position.Size() > 1)
	{
		result = position.GetEndTime();
		isSet = true;
	}

	if (rotation.Size() > 1)
	{
		float rotationStart = rotation.GetEndTime();
		if (rotationStart > result || !isSet)
		{
			result = rotationStart;
			isSet = true;
		}
	}

	if (scale.Size() > 1)
	{
		float scaleStart = scale.GetEndTime();
		if (scaleStart > result || !isSet)
		{
			result = scaleStart;
			isSet = true;
		}
	}

	return result;
}

template<typename VTRACK, typename QTRACK>
bool TTransformTrack<VTRACK, QTRACK>::IsValid() const
{
	return position.Size() > 1 || rotation.Size() > 1 || scale.Size() > 1;
}

template<typename VTRACK, typename QTRACK>
Math::Transform TTransformTrack<VTRACK, QTRACK>::Sample(const Math::Transform& refTransform, float time, bool isLooping)
{
	Math::Transform result = refTransform;

	if (position.Size() > 1)
	{
		result.Position() = position.Sample(time, isLooping);
	}

	if (rotation.Size() > 1)
	{
		result.Rotation() = rotation.Sample(time, isLooping);
	}

	if (scale.Size() > 1)
	{
		result.Scale() = scale.Sample(time, isLooping);
	}

	return result;
}

FastTransformTrack OptimizeTransformTrack(TransformTrack& input)
{
	FastTransformTrack result;

	result.SetId(input.GetId());
	result.GetPositionTrack() = OptimizeTrack<glm::vec3, 3>(input.GetPositionTrack());
	result.GetRotationTrack() = OptimizeTrack<glm::quat, 4>(input.GetRotationTrack());
	result.GetScaleTrack() = OptimizeTrack<glm::vec3, 3>(input.GetScaleTrack());

	return result;
}