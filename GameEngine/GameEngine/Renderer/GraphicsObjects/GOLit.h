#ifndef GOLIT_H
#define GOLIT_H

#include <glm/glm.hpp>

class Texture;

class GOLit
{
public:

	virtual ~GOLit();

	void SetShine(float shine);

protected:

	GOLit(Texture* const diffueseMap, Texture* const specularMap);

	GOLit(const GOLit&) = delete;

	GOLit& operator=(const GOLit&) = delete;

	GOLit(GOLit&&) = delete;

	GOLit& operator=(GOLit&&) = delete;

	void UpdateLighting();

private:

	struct AmbientUBO
	{
		glm::vec4 ambient;
	} ambient;

	struct DirectionalLightUBO {
		glm::vec4 color;
		glm::vec4 direction;
	} directionalLight;
	
	struct PointLightUBO {
		glm::vec4 color;
		glm::vec4 position;
		float constant;
		float linear;
		float quadratic;
	} pointLight;

	struct SpotLightUBO
	{
		glm::vec4 color;
		glm::vec4 position;
		glm::vec4 direction;
		float cutoff;
		float outerCutoff;
		float constant;
		float linear;
		float quadratic;
	} spotLight;

	struct ViewPositionUBO
	{
		glm::vec4 position;
	} viewPosition;

	struct MaterialUBO
	{
		float shine;
	} material;

	unsigned int ambientBuffer;

	unsigned int directionalLightBuffer;

	unsigned int pointLightBuffer;

	unsigned int spotLightBuffer;

	unsigned int viewPositionBuffer;

	unsigned int materialBuffer;

	Texture* specular;

	Texture* diffuse;
};

#endif // GOLIT_H