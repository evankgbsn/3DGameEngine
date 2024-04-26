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
		glm::vec4 color = glm::vec4(0.0f);
		glm::vec4 direction = glm::vec4(0.0f);
		bool lightOn = false;
		glm::vec3 pad; // Needed for padding. Otherwise multiples lights wont show because opengl will be 16 byte aligned.
	} directionalLight[20];
	
	struct PointLightUBO {
		glm::vec4 color = glm::vec4(0.0f);
		glm::vec4 position = glm::vec4(0.0f);
		float constant = 0.0f;
		float linear = 0.0f;
		float quadratic = 0.0f;
		bool lightOn = false;
	} pointLight[20];

	struct SpotLightUBO
	{
		glm::vec4 color = glm::vec4(0.0f);
		glm::vec4 position = glm::vec4(0.0f);
		glm::vec4 direction = glm::vec4(0.0f);
		float cutoff = 0.0f;
		float outerCutoff = 0.0f;
		float constant = 0.0f;
		float linear = 0.0f;
		float quadratic = 0.0f;
		bool lightOn = false;
		glm::vec2 pad; // Needed for padding. Otherwise multiples lights wont show because opengl will be 16 byte aligned.
	} spotLight[20];

	struct ViewPositionUBO
	{
		glm::vec4 position;
	} viewPosition;

	struct MaterialUBO
	{
		float shine;
		glm::vec3 pad; // Needed for padding. Otherwise multiples lights wont show because opengl will be 16 byte aligned.
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