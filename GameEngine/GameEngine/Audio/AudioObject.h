#ifndef AUDIOOBJECT_H
#define AUDIOOBJECT_H

#include <glm/glm.hpp>
#include <vector>

#include <phonon.h>

class Model;

class AudioObject
{
public:

	struct Material
	{
		float lowFrequencyAbsorption;
		float midFrequencyAbsorption;
		float highFrequencyAbsorption;

		float scattering;

		float lowFrequencyTransmission;
		float midFrequencyTransmission;
		float highFrequencyTransmission;
	};

	AudioObject(const Model* const model, const glm::vec3& position, const glm::mat4& rotation, const Material& material);

	void SetPosition(const glm::vec3& position);

	glm::vec3 GetPosition() const;

	void SetRotation(const glm::mat4& rotation);

	glm::mat4 GetRotation() const;

	const Model* const GetModel() const;

	void UpdateMaterial(const Material& newMat);

	const Material& GetMaterial() const;

private:

	friend class AudioManager;

	~AudioObject();

	AudioObject() = delete;

	AudioObject(const AudioObject&) = delete;

	AudioObject& operator=(const AudioObject&) = delete;

	AudioObject(AudioObject&&) = delete;

	AudioObject& operator=(AudioObject&&) = delete;

	const Model* const model;

	Material mat;

	glm::vec3 position;

	glm::mat4 rotation;

	std::vector<IPLVector3> vertices;

	std::vector<IPLTriangle> triangles;

	std::vector<IPLint32> materialIndices;

	IPLStaticMeshSettings steamStaticMeshSettings;

	IPLStaticMesh steamStaticMesh;

	IPLMaterial steamMaterial[1];

	IPLScene steamSubscene; // All dynaimc moveable objects need a subscene. This includes objects that are added and removed from the scene at runtime.
};

#endif //AUDIOOBJECT_H