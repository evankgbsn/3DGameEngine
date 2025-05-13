#ifndef AMBIENTLIGHT_H
#define AMBIENTLIGHT_H

#include "Component.h"

class AmbientLight : public Component
{

public:

	AmbientLight() = default;

	AmbientLight(float intensity);

	~AmbientLight();

	float GetIntensity() const;

	void SetAmbientIntensity(float intensity);

private:

	AmbientLight(const AmbientLight&) = delete;

	AmbientLight& operator=(const AmbientLight&) = delete;

	AmbientLight(AmbientLight&&) = delete;

	AmbientLight& operator=(AmbientLight&&) = delete;


	// Inherited via Component
	const std::vector<char> Serialize() const override;

	void Deserialize(const std::vector<char>& data) override;

	void Update() override;

};

#endif // AMBIENTLIGHT_H