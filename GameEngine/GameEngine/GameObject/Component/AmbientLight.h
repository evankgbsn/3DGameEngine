#ifndef AMBIENTLIGHT_H
#define AMBIENTLIGHT_H

class AmbientLight
{

public:

	AmbientLight(float intensity);

	~AmbientLight();

	float GetIntensity() const;

	void SetAmbientIntensity(float intensity);

private:

	AmbientLight(const AmbientLight&) = delete;

	AmbientLight& operator=(const AmbientLight&) = delete;

	AmbientLight(AmbientLight&&) = delete;

	AmbientLight& operator=(AmbientLight&&) = delete;

};

#endif // AMBIENTLIGHT_H