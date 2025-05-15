#ifndef AMBIENTLIGHT_H
#define AMBIENTLIGHT_H

#include "Component.h"

class AmbientLight : public Component
{

public:

	AmbientLight();

	AmbientLight(float intensity);

	~AmbientLight();

	float GetIntensity() const;

	void SetAmbientIntensity(float intensity);

private:

	AmbientLight(const AmbientLight&) = delete;

	AmbientLight& operator=(const AmbientLight&) = delete;

	AmbientLight(AmbientLight&&) = delete;

	AmbientLight& operator=(AmbientLight&&) = delete;

	void Serialize() override;

	void Deserialize() override;

	void Update() override;

};

#endif // AMBIENTLIGHT_H