#include "RigidBodyComponent.h"

#include "../../Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "../../Physics/RigidBody.h"
#include "../../Physics/PhysicsManager.h"
#include "../GameObject.h"
#include "../../GameEngine/Renderer/Model/Model.h"
#include "../../Collision/OrientedBoundingBoxWithVisualization.h"
#include "../../Renderer/GraphicsObjects/GOColored.h"
#include "../../Renderer/Model/ModelManager.h"

#include <cooking/PxCooking.h>

RigidBodyComponent::RigidBodyComponent(Type t, GameObject* owningObject, const Model* const m) :
	body(nullptr),
	owner(owningObject),
	model(m),
	type(t)
{

	CreateShapeFromModel();

	PxPlaneGeometry planeGeo = PxPlaneGeometry();

	PxConvexMeshGeometry convexGeo(convexMeshShape);
	PxTriangleMeshGeometry triangleGeo(triangleMeshShape);

	switch (type)
	{
	case Type::STATIC:
		body = new RigidBody(RigidBody::Type::STATIC, &triangleGeo, owningObject->GetPosition(), owningObject->GetRotation());
		break;
	case Type::DYNAMIC:
		body = new RigidBody(RigidBody::Type::DYNAMIC, &convexGeo, owningObject->GetPosition(), owningObject->GetRotation());
		break;
	default:
		break;
	}
}

RigidBodyComponent::~RigidBodyComponent()
{
	delete body;
}

void RigidBodyComponent::SyncPhysics()
{
	owner->SetPosition(body->GetPosition());
	owner->SetRotation(body->GetRotation());
	shapeVisuals->SetTransform(owner->GetTransform());
}

void RigidBodyComponent::CreateShapeFromModel()
{
	const std::vector<Vertex>& modelVerts = model->GetVertices();
	const std::vector<unsigned int>& modelIndices = model->GetIndices();

	std::vector<PxVec3> vertCloud;

	for (const Vertex& vert : modelVerts)
	{
		vertCloud.push_back(PxVec3(vert.GetPosition().x, vert.GetPosition().y, vert.GetPosition().z));
	}


	if (type == Type::DYNAMIC)
	{
		PxConvexMeshDesc convexDesc;
		convexDesc.points.count = vertCloud.size();
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

		shapeVisuals = GraphicsObjectManager::CreateGO3DColored(ModelManager::LoadModel("CollisionReference", convexShapeVerts, convexShapeIndices), { 1.0f, 0.0f, 0.0f, 1.0f });
		shapeVisuals->SetDrawMode(GO3D::Mode::POINT);
		shapeVisuals->SetPointSize(10.0f);
	}
	else if (type == Type::STATIC)
	{
		PxTriangleMeshDesc meshDesc;
		meshDesc.points.count = vertCloud.size();
		meshDesc.points.stride = sizeof(PxVec3);
		meshDesc.points.data = vertCloud.data();
		meshDesc.triangles.count = modelIndices.size() / 3;
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

		shapeVisuals = GraphicsObjectManager::CreateGO3DColored(ModelManager::LoadModel("StaticCollisionReference", triangleMeshVerts, triangleMeshIndices), { 1.0f, 0.0f, 0.0f, 1.0f });
		shapeVisuals->SetDrawMode(GO3D::Mode::LINE);
		shapeVisuals->SetPointSize(10.0f);
	}
	
}

const std::vector<char> RigidBodyComponent::Serialize() const
{
	return std::vector<char>();
}

void RigidBodyComponent::Deserialize(const std::vector<char>& data)
{
}

void RigidBodyComponent::Update()
{
}
