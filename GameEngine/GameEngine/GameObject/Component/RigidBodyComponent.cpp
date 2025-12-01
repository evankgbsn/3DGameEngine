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
	if (owner != nullptr)
	{
		owner->SetPosition(body->GetPosition());
		owner->SetRotation(body->GetRotation());
		shapeVisuals->SetTransform(owner->GetTransform());
	}
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

void RigidBodyComponent::SetRotation(const glm::mat4& newRot)
{
	body->SetRotation(newRot);
	shapeVisuals->SetRotation(newRot);
}

bool RigidBodyComponent::Hovered() const
{
	return body->Hovered();
}

void RigidBodyComponent::SetOwner(GameObject* newOwner)
{
	owner = newOwner;

	body->SetUserData(static_cast<void*>(owner));
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

		// 2. Access the core geometry data
		const PxU32 nbVertices = convexMeshShape->getNbVertices();
		const PxVec3* vertices = convexMeshShape->getVertices();
		const PxU8* indexBuffer = convexMeshShape->getIndexBuffer();
		// Safest reference: Cast the raw buffer to the PxU16 type once.
		const PxU16* PxU16_IndexBuffer = reinterpret_cast<const PxU16*>(indexBuffer);
		PxU32 totalIndexCount = 0;

		// 1. Get the total number of polygons (faces)
		const PxU32 nbPolygons = convexMeshShape->getNbPolygons();

		// 2. Iterate through each polygon by index
		for (PxU32 i = 0; i < nbPolygons; ++i) {
			PxHullPolygon polygonData;

			// Retrieve the specific polygon data by its index
			if (convexMeshShape->getPolygonData(i, polygonData)) {
				// mNbVerts is the number of vertices (and thus indices) for this polygon
				totalIndexCount += polygonData.mNbVerts;
			}
			else {
				// Handle error: Failed to retrieve polygon data
				break;
			}
		}

		// totalIndexCount now holds the sum of all polygon vertices
		// (before triangulation, if needed)

		for (PxU32 i = 0; i < nbPolygons; ++i) {
			PxHullPolygon polyData;
			if (convexMeshShape->getPolygonData(i, polyData)) {

				const PxU32 faceVertexCount = polyData.mNbVerts;
				const PxU32 indexBase = polyData.mIndexBase; // Byte offset

				// -----------------------------------------------------------
				// CRITICAL FIX FOR PxU8: The indexBase (byte offset) IS the element offset.
				// The indices start at the exact byte offset in the raw buffer.
				const PxU8* faceIndices = indexBuffer + indexBase;
				// -----------------------------------------------------------

				if (faceVertexCount < 3) continue;

				// V0 is the fan center, read directly as a PxU8 value.
				// We cast the PxU8 to PxU32 for the final OpenGL buffer.
				const PxU32 V0 = faceIndices[0];

				// Fan Triangulation loop
				for (PxU32 j = 1; j < faceVertexCount - 1; ++j) {

					const PxU32 V1 = faceIndices[j];
					const PxU32 V2 = faceIndices[j + 1];

					// Add the three indices (V0, V2, V1) 
					convexShapeIndices.push_back(V0);
					convexShapeIndices.push_back(V1);
					convexShapeIndices.push_back(V2);
				}
			}
		}

		for (unsigned int i = 0; i < convexMeshShape->getNbVertices(); i++)
		{
			const PxVec3& vert = convexMeshShape->getVertices()[i];
			convexShapeVerts.push_back(Vertex({ vert.x, vert.y, vert.z }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f }));
			//convexShapeIndices.push_back(i);
		}

		modelNameDynamic = "CollisionReference" + std::to_string(x++);

		ModelManager::LoadModel(modelNameDynamic, convexShapeVerts, convexShapeIndices, false);

		shapeVisuals = GraphicsObjectManager::CreateGO3DColored(ModelManager::GetModel(modelNameDynamic), {0.0f, 0.5f, 0.5f, 1.0f});
		shapeVisuals->SetDrawMode(GO3D::Mode::LINE);
		shapeVisuals->SetLineWidth(5.0f);
		shapeVisuals->SetPointSize(8.0f);
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

		std::vector<physx::PxVec3> outVertices;
		std::vector<physx::PxU32> outIndices;

		// --- 1. Get Vertices (PxVec3 is 3x float) ---
		const physx::PxU32 nbVertices = triangleMeshShape->getNbVertices();
		const physx::PxVec3* vertices = triangleMeshShape->getVertices();
		outVertices.assign(vertices, vertices + nbVertices);

		// --- 2. Get Triangles (Indices) ---
		const physx::PxU32 nbTriangles = triangleMeshShape->getNbTriangles();

		// Check for the 16-bit flag. If SET, use PxU16. If NOT SET, use PxU32.
		const bool has16BitIndices = triangleMeshShape->getTriangleMeshFlags() & physx::PxTriangleMeshFlag::e16_BIT_INDICES;

		// The raw pointer to the index array
		const void* indexData = triangleMeshShape->getTriangles();
		const physx::PxU32 totalIndices = nbTriangles * 3;

		outIndices.reserve(totalIndices);

		if (has16BitIndices)
		{
			// 16-bit indices (PxU16)
			const physx::PxU16* indices16 = static_cast<const physx::PxU16*>(indexData);

			// Convert PxU16 to PxU32 for the output vector
			for (physx::PxU32 i = 0; i < totalIndices; ++i)
			{
				outIndices.push_back(static_cast<physx::PxU32>(indices16[i]));
			}
		}
		else
		{
			// 32-bit indices (PxU32)
			const physx::PxU32* indices32 = static_cast<const physx::PxU32*>(indexData);

			// Copy directly
			outIndices.assign(indices32, indices32 + totalIndices);
		}

		for (const auto& vert : outVertices)
		{
			triangleMeshVerts.push_back(Vertex({ vert.x, vert.y, vert.z }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f }));
		}

		for (const auto& index : outIndices)
		{
			triangleMeshIndices.push_back(index);
		}

		modelNameStatic = "CollisionReference" + std::to_string(x++);

		ModelManager::LoadModel(modelNameStatic, triangleMeshVerts, triangleMeshIndices, false);

		shapeVisuals = GraphicsObjectManager::CreateGO3DColored(ModelManager::GetModel(modelNameStatic), {0.0f, 0.5f, 0.5f, 1.0f});
		shapeVisuals->SetDrawMode(GO3D::Mode::LINE);
		shapeVisuals->SetLineWidth(5.0f);
		shapeVisuals->SetPointSize(8.0f);
	}

	if (!Editor::IsEnabled())
	{
		GraphicsObjectManager::Disable(shapeVisuals);
	}
}

void RigidBodyComponent::Serialize()
{
	savedStrings["ModelName"] = model->GetName();
	savedBools["IsDynamic"] = type == Type::DYNAMIC;
	savedVec3s["Position"] = body->GetPosition();
	savedMat4s["Rotation"] = body->GetRotation();
}

void RigidBodyComponent::Deserialize()
{
	model = ModelManager::GetModel(savedStrings["ModelName"]);
	type = savedBools["IsDynamic"] ? Type::DYNAMIC : Type::STATIC;

	CreateShapeFromModel();
	
	CreatePhysXRigidBodyWithoutOwner(savedVec3s["Position"], savedMat4s["Rotation"]);
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

	shapeVisuals->SetTranslation(owner->GetPosition());
	shapeVisuals->SetRotation(owner->GetRotation());

	body->SetUserData(static_cast<void*>(owner));
}

void RigidBodyComponent::CreatePhysXRigidBodyWithoutOwner(const glm::vec3& position, const glm::mat4& rotation)
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
		body = new RigidBody(RigidBody::Type::STATIC, &triangleGeo, position, rotation);
		break;
	case Type::DYNAMIC:
		body = new RigidBody(RigidBody::Type::DYNAMIC, &convexGeo, position, rotation);
		break;
	default:
		break;
	}

	SetPosition(position);
	SetRotation(rotation);
}
