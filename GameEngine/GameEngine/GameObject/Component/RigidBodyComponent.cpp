#include "RigidBodyComponent.h"

#include "../../Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "../../Physics/RigidBody.h"
#include "../../Physics/PhysicsManager.h"
#include "../GameObject.h"
#include "../../GameEngine/Renderer/Model/Model.h"
#include "../../Collision/OrientedBoundingBoxWithVisualization.h"
#include "../../Renderer/GraphicsObjects/GOColored.h"
#include "../../Renderer/Model/ModelManager.h"
#include "../../Editor/Editor.h"
#include "../../Scene/SceneManager.h"

#include <cooking/PxCooking.h>

RigidBodyComponent::RigidBodyComponent() :
	body(nullptr),
	owner(nullptr),
	model(nullptr),
	type(Type::DYNAMIC),
	onEditorEnable(nullptr),
	onEditorDisable(nullptr),
	shapeVisuals(nullptr),
	modelLoadCallbackDynamic(nullptr),
	modelLoadCallbackStatic(nullptr)
{
	RegisterComponentClassType<RigidBodyComponent>(this);
	RegisterEditorCallbacks();
}

RigidBodyComponent::RigidBodyComponent(Type t, GameObject* owningObject, const Model* m) :
	body(nullptr),
	owner(owningObject),
	model(m),
	type(t),
	onEditorEnable(nullptr),
	onEditorDisable(nullptr),
	modelLoadCallbackDynamic(nullptr),
	modelLoadCallbackStatic(nullptr)
{
	RegisterComponentClassType<RigidBodyComponent>(this);
	CreateShapeFromModel();
	CreatePhysXRigidBody();
	RegisterEditorCallbacks();
}

RigidBodyComponent::~RigidBodyComponent()
{
	if (modelLoadCallbackDynamic != nullptr)
	{
		ModelManager::DergisterCallbackForModelLoaded(modelNameDynamic, "RigidBodyDynamicGraphics");
		delete modelLoadCallbackDynamic;
	}

	if (modelLoadCallbackStatic != nullptr)
	{
		ModelManager::DergisterCallbackForModelLoaded(modelNameStatic, "RigidBodyStaticGraphics");
		delete modelLoadCallbackStatic;
	}
	

	Editor::DeregisterOnEditorEnable(onEditorEnable);
	Editor::DeregisterOnEditorDisable(onEditorDisable);

	delete onEditorEnable;
	delete onEditorDisable;
	delete body;

	if (shapeVisuals != nullptr)
	{
		GraphicsObjectManager::Delete(shapeVisuals);
	}
}

void RigidBodyComponent::SyncPhysics()
{
	owner->SetPosition(body->GetPosition());
	owner->SetRotation(body->GetRotation());
	shapeVisuals->SetTransform(owner->GetTransform());
}

void RigidBodyComponent::SyncPhysicsPosition()
{
	if (shapeVisuals != nullptr)
	{
		shapeVisuals->SetTransform(owner->GetTransform());
	}

	owner->SetPosition(body->GetPosition());
}

void RigidBodyComponent::SyncPhysicsRotation()
{
	if (shapeVisuals != nullptr)
	{
		shapeVisuals->SetTransform(owner->GetTransform());
	}

	owner->SetRotation(body->GetRotation());
}

void RigidBodyComponent::LockAngularMotionOnAxisX()
{
	body->LockAngularMotionOnAxisX();
}

void RigidBodyComponent::LockAngularMotionOnAxisY()
{
	body->LockAngularMotionOnAxisY();
}

void RigidBodyComponent::LockAngularMotionOnAxisZ()
{
	body->LockAngularMotionOnAxisZ();
}

void RigidBodyComponent::LockLinearMotionOnAxisX()
{
	body->LockLinearMotionOnAxisX();
}

void RigidBodyComponent::LockLinearMotionOnAxisY()
{
	body->LockLinearMotionOnAxisY();
}

void RigidBodyComponent::LockLinearMotionOnAxisZ()
{
	body->LockLinearMotionOnAxisZ();
}

void RigidBodyComponent::AddForce(const glm::vec3& direction)
{
	body->AddForce(direction);
}

glm::vec3 RigidBodyComponent::GetVelocity() const
{
	return body->GetVelocity();
}

void RigidBodyComponent::SetPosition(const glm::vec3& newPosition)
{
	body->SetPosition(newPosition);
	shapeVisuals->SetTranslation(newPosition);
}

bool RigidBodyComponent::Hovered() const
{
	return body->Hovered();
}

void RigidBodyComponent::CreateShapeFromModel()
{
	if (shapeVisuals != nullptr)
	{
		GraphicsObjectManager::Delete(shapeVisuals);
	}

	const std::vector<Vertex>& modelVerts = model->GetVertices();
	const std::vector<unsigned int>& modelIndices = model->GetIndices();

	std::vector<PxVec3> vertCloud;

	for (const Vertex& vert : modelVerts)
	{
		vertCloud.push_back(PxVec3(vert.GetPosition().x, vert.GetPosition().y, vert.GetPosition().z));
	}

	static int x = 0;

	if (type == Type::DYNAMIC)
	{
		PxConvexMeshDesc convexDesc;
		convexDesc.points.count = static_cast<PxU32>(vertCloud.size());
		convexDesc.points.stride = sizeof(PxVec3);
		convexDesc.points.data = vertCloud.data();
		convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;
		convexDesc.vertexLimit = 255;

		PxTolerancesScale scale;
		PxCookingParams params(scale);

		PxDefaultMemoryOutputStream buf;
		PxConvexMeshCookingResult::Enum result;
		if (!PxCookConvexMesh(params, convexDesc, buf, &result))
			return;
		PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
		convexMeshShape = PhysicsManager::GetPhysics()->createConvexMesh(input);

		std::vector<Vertex> convexShapeVerts;
		std::vector<unsigned int> convexShapeIndices;

		for (unsigned int i = 0; i < convexMeshShape->getNbVertices(); i++)
		{
			const PxVec3& vert = convexMeshShape->getVertices()[i];
			convexShapeVerts.push_back(Vertex({ vert.x, vert.y, vert.z }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f }));
			convexShapeIndices.push_back(i);
		}

		modelLoadCallbackDynamic = new std::function<void(Model* const)>([this](Model* const model)
			{
				shapeVisuals = GraphicsObjectManager::CreateGO3DColored(model, { 0.0f, 0.5f, 0.5f, 1.0f });
				shapeVisuals->SetDrawMode(GO3D::Mode::POINT);
				shapeVisuals->SetPointSize(4.0f);
			});

		modelNameDynamic = "CollisionReference" + std::to_string(x++);

		ModelManager::RegisterCallbackForModelLoaded(modelNameDynamic, "RigidBodyDynamic", modelLoadCallbackDynamic);

		ModelManager::LoadModel(modelNameDynamic, convexShapeVerts, convexShapeIndices, true);
	}
	else if (type == Type::STATIC)
	{
		PxTriangleMeshDesc meshDesc;
		meshDesc.points.count = static_cast<PxU32>(vertCloud.size());
		meshDesc.points.stride = sizeof(PxVec3);
		meshDesc.points.data = vertCloud.data();
		meshDesc.triangles.count = static_cast<PxU32>(modelIndices.size() / 3);
		meshDesc.triangles.stride = 3 * sizeof(PxU32);
		meshDesc.triangles.data = modelIndices.data();

		PxTolerancesScale scale;
		PxCookingParams params(scale);

		PxDefaultMemoryOutputStream writeBuffer;
		PxTriangleMeshCookingResult::Enum result;
		bool status = PxCookTriangleMesh(params, meshDesc, writeBuffer, &result);
		if (!status)
			return;

		PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
		triangleMeshShape = PhysicsManager::GetPhysics()->createTriangleMesh(readBuffer);

		const PxU16* triangleIndices = static_cast<const PxU16*>(triangleMeshShape->getTriangles());

		PxU32 count = triangleMeshShape->getNbTriangles();

		std::vector<Vertex> triangleMeshVerts;
		std::vector<unsigned int> triangleMeshIndices;

		for (unsigned int i = 0; i < triangleMeshShape->getNbTriangles(); i++)
		{

			triangleMeshIndices.push_back(triangleIndices[0 + i * 3]);
			triangleMeshIndices.push_back(triangleIndices[1 + i * 3]);
			triangleMeshIndices.push_back(triangleIndices[2 + i * 3]);
		}

		const PxVec3* verts = triangleMeshShape->getVertices();

		for (unsigned int i = 0; i < triangleMeshShape->getNbVertices(); i++)
		{
			PxVec3 vert = verts[i];
			glm::vec3 glmVert(vert.x, vert.y, vert.z);

			triangleMeshVerts.push_back(Vertex(glmVert, {}, {}));
		}

		modelLoadCallbackStatic = new std::function<void(Model* const)>([this](Model* const model)
			{
				shapeVisuals = GraphicsObjectManager::CreateGO3DColored(model, { 0.0f, 0.5f, 0.5f, 1.0f });
				shapeVisuals->SetDrawMode(GO3D::Mode::POINT);
				shapeVisuals->SetPointSize(4.0f);

				if (!Editor::IsEnabled())
				{
					GraphicsObjectManager::Disable(shapeVisuals);
				}
			});

		modelNameStatic = "CollisionReference" + std::to_string(x++);

		ModelManager::RegisterCallbackForModelLoaded(modelNameStatic, "RigidBodyDynamic", modelLoadCallbackDynamic);

		ModelManager::LoadModel(modelNameStatic, triangleMeshVerts, triangleMeshIndices, true);
	}
}

void RigidBodyComponent::Serialize()
{
	savedStrings["ModelName"] = model->GetName();
	savedStrings["OwningObject"] = owner->GetName();
	savedBools["IsDynamic"] = type == Type::DYNAMIC;
}

void RigidBodyComponent::Deserialize()
{
	owner = SceneManager::FindGameObject(savedStrings["OwningObject"]);
	model = ModelManager::GetModel(savedStrings["ModelName"]);
	type = savedBools["IsDynamic"] ? Type::DYNAMIC : Type::STATIC;

	CreateShapeFromModel();
	CreatePhysXRigidBody();
}

void RigidBodyComponent::Update()
{
}

void RigidBodyComponent::RegisterEditorCallbacks()
{
	onEditorEnable = new std::function<void()>([this]()
		{
			if (shapeVisuals != nullptr)
			{
				GraphicsObjectManager::Enable(shapeVisuals);
			}
		});

	onEditorDisable = new std::function<void()>([this]()
		{
			if (shapeVisuals != nullptr)
			{
				GraphicsObjectManager::Disable(shapeVisuals);
			}
		});

	Editor::RegisterOnEditorEnable(onEditorEnable);
	Editor::RegisterOnEditorDisable(onEditorDisable);
}

void RigidBodyComponent::CreatePhysXRigidBody()
{
	if (body != nullptr)
	{
		delete body;
	}

	PxPlaneGeometry planeGeo = PxPlaneGeometry();

	PxConvexMeshGeometry convexGeo(convexMeshShape);
	PxTriangleMeshGeometry triangleGeo(triangleMeshShape);

	switch (type)
	{
	case Type::STATIC:
		body = new RigidBody(RigidBody::Type::STATIC, &triangleGeo, owner->GetPosition(), owner->GetRotation());
		break;
	case Type::DYNAMIC:
		body = new RigidBody(RigidBody::Type::DYNAMIC, &convexGeo, owner->GetPosition(), owner->GetRotation());
		break;
	default:
		break;
	}

	body->SetUserData(static_cast<void*>(owner));
}
