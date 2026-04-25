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

	struct alignas(16) AmbientUBO
	{
		glm::vec4 ambient;
	} ambient;

	struct alignas(16) DirectionalLightUBO {
		glm::vec4 color = glm::vec4(0.0f);
		glm::vec4 direction = glm::vec4(0.0f);
		int lightOn = 0;
		glm::vec3 pad; // Needed for padding. Otherwise multiples lights wont show because opengl will be 16 byte aligned.
	} directionalLight[20];

	struct alignas(16) PointLightUBO {
		glm::vec4 color = glm::vec4(0.0f);
		glm::vec4 position = glm::vec4(0.0f);
		float constant = 0.0f;
		float linear = 0.0f;
		float quadratic = 0.0f;
		int lightOn = 0;
	} pointLight[20];

	struct alignas(16) SpotLightUBO
	{
		// These naturally align to 16-byte boundaries
		glm::vec4 color = glm::vec4(0.0f);     // Offset 0
		glm::vec4 position = glm::vec4(0.0f);  // Offset 16
		glm::vec4 direction = glm::vec4(0.0f); // Offset 32

		// This block starts at offset 48
		float cutoff;       // Offset 48
		float outerCutoff;  // Offset 52
		float constant;     // Offset 56
		float linear;       // Offset 60

		// This block starts at offset 64
		float quadratic;    // Offset 64
		int lightOn;        // Offset 68
		glm::vec2 pad;      // Offset 72-79 (Crucial: Pads struct to 80 bytes)
	} spotLight[20];

	struct alignas(16) ViewPositionUBO
	{
		glm::vec4 position;
	} viewPosition;

	struct alignas(16) MaterialUBO
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