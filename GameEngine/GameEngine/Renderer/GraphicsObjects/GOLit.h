#ifndef GOLIT_H
#define GOLIT_H

#include <glm/glm.hpp>
#include <vector>

class Texture;

class GOLit
{
public:

	struct Material
	{
		Material(Texture* const diff, Texture* const spec, Texture* const normal) :
			diffuseMap(diff),
			specularMap(spec),
			normalMap(normal)
		{

		};

		Texture* const diffuseMap;
		Texture* const specularMap;
		Texture* const normalMap;
	};

	virtual ~GOLit();

	void SetShine(float shine);

	float GetShine() const;

	const std::vector<GOLit::Material>& GetMaterials() const;

protected:

	GOLit(const std::vector<Material>& materials);

	GOLit(const GOLit&) = delete;

	GOLit& operator=(const GOLit&) = delete;

	GOLit(GOLit&&) = delete;

	GOLit& operator=(GOLit&&) = delete;

	void UpdateLighting();

	std::vector<Material> materials;

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
		int lightOn = 0; // Changed to int for 4-byte alignment
		glm::vec2 pad;   // Pad to 16-byte boundary (6 floats/ints total in this block)
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

	unsigned int fogBuffer;

	struct FogEffect
	{
		float fogDensity;
		float fogGradient;
	} fog;

	glm::vec4 lastViewPos;

	glm::vec4 lastAmbient;

	float lastfogDensity;

	float lastFogGradient;
	
	float lastShine;

};

#endif // GOLIT_H