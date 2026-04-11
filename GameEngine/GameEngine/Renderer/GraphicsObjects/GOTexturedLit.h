#ifndef GOTEXTUREDLIT_H
#define GOTEXTUREDLIT_H

#include "GO3D.h"
#include "GOLit.h"

#include <unordered_map>
#include <list>

class Model;
class Texture;
class GOTexturedLitInstanced;

class GOTexturedLit : public GO3D, public GOLit
{
public:

	GOTexturedLit(Model* const model, const GOLit::Material& material);

	~GOTexturedLit();

	void Update() override;

	void RenderToShadowMap() override;

	Texture* const GetDiffuseTexture() const;

	Texture* const GetSpecularTexture() const;

	Texture* const GetNormalTexture() const;

	virtual void Translate(const glm::vec3& t) override;

	virtual void Rotate(float angle, const glm::vec3& axis) override;

	virtual void Scale(const glm::vec3& s) override;

	virtual void SetTranslation(const glm::vec3& t) override;

	virtual void SetRotation(const glm::mat4& newRotation) override;

	virtual void SetScale(const glm::vec3& s) override;

	virtual glm::vec3 GetTranslation() const override;

	virtual glm::vec3 GetScale() const override;

	virtual glm::mat4 GetRotation() const override;

	virtual void SetTransform(const glm::mat4& newTransform) override;

	virtual const glm::mat4& GetTransform() override;

protected:

private:

	GOTexturedLit();

	GOTexturedLit(const GOTexturedLit&) = delete;

	GOTexturedLit& operator=(const GOTexturedLit&) = delete;

	GOTexturedLit(GOTexturedLit&&) = delete;

	GOTexturedLit& operator=(GOTexturedLit&&) = delete;

	unsigned int lightSpaceMatrixBuffer;

	const std::string instanceIdentifier;

	unsigned int instanceID = 1000;

	bool instanced = false;

	static std::unordered_map<std::string, GOTexturedLitInstanced*> instancedGraphicsObjects;

	static std::unordered_map<std::string, std::list<GOTexturedLit*>> instances;

};

#endif // GOTEXTUREDLIT_H