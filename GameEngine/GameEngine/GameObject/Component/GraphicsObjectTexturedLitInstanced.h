#ifndef GRAPHICSOBJECTTEXTUREDLITINSTANCED_H
#define GRAPHICSOBJECTTEXTUREDLITINSTANCED_H

#include "Component.h"

#include <string>

class GOTexturedLitInstanced;

class GraphicsObjectTexturedLitInstanced : public Component
{
public:


	GraphicsObjectTexturedLitInstanced();

	GraphicsObjectTexturedLitInstanced(const std::string& modelName, const std::string& diffuseTextureName, const std::string& specularTextureName, unsigned int instanceCount);

	~GraphicsObjectTexturedLitInstanced();

	void Translate(const glm::vec3& t, unsigned int instanceID);

	void Rotate(float angle, const glm::vec3& axis, unsigned int instanceID);

	void Scale(const glm::vec3& s, unsigned int instanceID);

	void SetTranslation(const glm::vec3& t, unsigned int instanceID);

	void SetRotation(const glm::mat4& newRotation, unsigned int instanceID);

	void SetScale(const glm::vec3& s, unsigned int instanceID);

	glm::vec3 GetTranslation(unsigned int instanceID) const;

	glm::vec3 GetScale(unsigned int instanceID) const;

	glm::mat4 GetRotation(unsigned int instanceID) const;

	void SetTransform(const glm::mat4& newTransform, unsigned int instanceID);

	glm::mat4 GetTransform(unsigned int instanceID) const;

	void Update() override;

	unsigned int AddInstance();

	void RemoveInstanceByID(unsigned int instanceID);

	unsigned int GetInstanceCount();

	void FinalizeTransforms();

	void UpdateInstanceByID(unsigned int instanceID);

	void SetShine(float shine);

	GOTexturedLitInstanced* GetGraphics() const;

private:

	GraphicsObjectTexturedLitInstanced(const GraphicsObjectTexturedLitInstanced&) = delete;

	GraphicsObjectTexturedLitInstanced& operator=(const GraphicsObjectTexturedLitInstanced&) = delete;

	GraphicsObjectTexturedLitInstanced(GraphicsObjectTexturedLitInstanced&&) = delete;

	GraphicsObjectTexturedLitInstanced& operator=(GraphicsObjectTexturedLitInstanced&&) = delete;

	void Serialize() override;

	void Deserialize() override;

	GOTexturedLitInstanced* graphics;

};

#endif // GRAPHICSOBJECTTEXTUREDLITINSTANCED_H