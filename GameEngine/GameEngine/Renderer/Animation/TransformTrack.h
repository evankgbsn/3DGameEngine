#ifndef TRANSFORMTRACK_H
#define TRANSFORMTRACK_H

#include "Track.h"
#include "../../Math/Transform.h"

template<typename VTRACK, typename QTRACK>
class TTransformTrack
{

public:

	TTransformTrack();

	~TTransformTrack();

	TTransformTrack(const TTransformTrack&) = default;

	TTransformTrack& operator=(const TTransformTrack&) = default;

	TTransformTrack(TTransformTrack&&) = default;

	TTransformTrack& operator=(TTransformTrack&&) = default;

	unsigned int GetId() const;

	void SetId(const unsigned int& newId);

	VTRACK& GetPositionTrack();

	QTRACK& GetRotationTrack();

	VTRACK& GetScaleTrack();

	float GetStartTime() const;

	float GetEndTime() const;

	bool IsValid() const;

	Math::Transform Sample(const Math::Transform& transform, float time, bool isLooping);

protected:

	unsigned int id;

	VTRACK position;
	
	QTRACK rotation;
	
	VTRACK scale;

};

typedef TTransformTrack<VectorTrack, QuaternionTrack> TransformTrack;
typedef TTransformTrack<VectorFastTrack, QuaternionFastTrack> FastTransformTrack;

FastTransformTrack OptimizeTransformTrack(TransformTrack& input);

#endif // TRANSFORMTRACK_H