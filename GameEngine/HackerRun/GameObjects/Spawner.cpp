#include "Spawner.h"

#include "GameEngine/Scene/Scene.h"
#include "GameEngine/Time/TimeManager.h"

#include "Corruption.h"
#include "MemoryBlock.h"
#include "HackerRunner.h"
#include "GameEngine/GameObject/Component/GraphicsObjectLine.h"

Spawner::Spawner() :
	GameObject("Spawner"),
	corruptionSpawnInterval(3.0f),
	maxCorruptionCount(15U),
	spaceBetweenBlocks(150.0f),
    radius(3),
    lineDataSpeed(1000.0f)
{
	RegisterGameObjectClassType<Spawner>(this);
}

Spawner::~Spawner()
{
	for (Corruption* c : corruptionObjects)
	{
		c->GetOwningScene()->DeregisterGameObject(c->GetName(), [](GameObject* obj) {delete static_cast<Corruption*>(obj);});
	}
}

void Spawner::Initialize()
{
    
}

void Spawner::Terminate()
{
    for (GraphicsObjectLine* line : dataLines)
    {
        delete line;
    }

    for (const auto& row : blocks)
    {
        for (MemoryBlock* block : row)
        {
            GetOwningScene()->DeregisterGameObject(std::to_string((long long)block), [](GameObject* obj) {delete static_cast<MemoryBlock*>(obj); });
        }
    }
}

void Spawner::GameUpdate()
{
	if (TimeManager::SecondsSinceStart() - lastCorruptionSpawnTime > corruptionSpawnInterval && corruptionObjects.size() < maxCorruptionCount)
	{
		Corruption* newCorruption = new Corruption();
		GetOwningScene()->RegisterGameObject(newCorruption, std::to_string((long long)newCorruption));

		lastCorruptionSpawnTime = TimeManager::SecondsSinceStart();
	}

	HackerRunner* runner = nullptr;
	for (const auto& obj : GetOwningScene()->GetGameObjects())
	{
		if ((runner = dynamic_cast<HackerRunner*>(obj.second)) != nullptr)
		{
			break;
		}
	}

    // ADD THIS: Stop running the spawner logic if the player doesn't exist this frame
    if (runner == nullptr)
    {
        return;
    }

    glm::vec3 pos = runner->GetPosition();

    // 1. USE SIGNED INTEGERS!
    glm::ivec2 currentBlockPos = glm::ivec2(std::floor(pos.x / spaceBetweenBlocks),
        std::floor(pos.z / spaceBetweenBlocks));

    if (blockPos != currentBlockPos)
    {
        // 2. Define your grid parameters
        // A radius of 2 means a 5x5 grid (Center + 2 in all directions)
        int gridWidth = (radius * 2) + 1; // e.g., 5
        float totalGridSize = gridWidth * spaceBetweenBlocks;

        // 3. Loop through your 2D vector (assuming it's called 'blockGrid')
        for (auto& row : blocks)
        {
            for (auto& block : row)
            {
                if (block == nullptr) continue;

                glm::vec3 bPos = block->GetPosition();

                // Get this block's current grid coordinate
                glm::ivec2 bGridPos = glm::ivec2(std::round(bPos.x / spaceBetweenBlocks),
                    std::round(bPos.z / spaceBetweenBlocks));

                // 4. Wrap the X axis
                if (bGridPos.x < currentBlockPos.x - radius)
                {
                    bPos.x += totalGridSize; // Jump to the right edge
                }
                else if (bGridPos.x > currentBlockPos.x + radius)
                {
                    bPos.x -= totalGridSize; // Jump to the left edge
                }

                // 5. Wrap the Z axis
                if (bGridPos.y < currentBlockPos.y - radius)
                {
                    bPos.z += totalGridSize; // Jump forward
                }
                else if (bGridPos.y > currentBlockPos.y + radius)
                {
                    bPos.z -= totalGridSize; // Jump backward
                }

                // 6. Apply the new position if it changed
                if (bPos != block->GetPosition())
                {
                    block->SetPosition(bPos);

                    // PhysX Note: Because you are teleporting the block, 
                    // you must update the rigid body directly so PhysX doesn't 
                    // think it moved at light-speed and smash other objects.
                    //block->SyncPhysics(); 
                }
            }
        }

        // 7. Update the stored position so we don't trigger this every frame
        blockPos = currentBlockPos;
    }

    float halfSpace = spaceBetweenBlocks / 2.0f;

    // 2. Get the player's forward direction from their rotation matrix
    glm::mat4 rotation = runner->GetRotation();
    glm::vec3 forward = glm::normalize(glm::vec3(rotation[2]));

    // 3. Determine the primary viewing axis
    // If the X component is larger than the Z component, they are looking sideways
    bool lookingAlongX = std::abs(forward.x) > std::abs(forward.z);

    if (dataLines.empty())
    {
        glm::vec3 startPoint, endPoint;

        if (lookingAlongX)
        {
            // Snap Z to nearest gap, project line along the X-axis
            float nearestGapZ = std::round((pos.z - halfSpace) / spaceBetweenBlocks) * spaceBetweenBlocks + halfSpace;
            float dirSign = (forward.x > 0.0f) ? 1.0f : -1.0f; // Preserve whether they are looking Left or Right

            startPoint = glm::vec3(pos.x - (1000.0f * dirSign), 0.0f, nearestGapZ);
            endPoint = glm::vec3(pos.x + (1000.0f * dirSign), 0.0f, nearestGapZ);
        }
        else
        {
            // Snap X to nearest gap, project line along the Z-axis
            float nearestGapX = std::round((pos.x - halfSpace) / spaceBetweenBlocks) * spaceBetweenBlocks + halfSpace;
            float dirSign = (forward.z > 0.0f) ? 1.0f : -1.0f; // Preserve whether they are looking Forward or Backward

            startPoint = glm::vec3(nearestGapX, 0.0f, pos.z - (1000.0f * dirSign));
            endPoint = glm::vec3(nearestGapX, 0.0f, pos.z + (1000.0f * dirSign));
        }

        dataLines.push_back(new GraphicsObjectLine(startPoint, endPoint, { 0.5f, 0.9f, 0.9f, 1.0f }));
    }
    else
    {
        GraphicsObjectLine* line = dataLines.front();

        line->SetStart(line->GetStart() + line->GetDirection() * TimeManager::DeltaTime() * lineDataSpeed);
        line->SetEnd(line->GetEnd() + line->GetDirection() * TimeManager::DeltaTime() * lineDataSpeed);

        float recycleDistance = 2000.0f;

        // 4. Direction-Agnostic Boundary Check
        // By using the dot product against the line's travel direction, we can measure how 
        // far away the line is relative to the player, regardless of which way it is moving.
        float distanceToStart = glm::dot(line->GetStart() - pos, line->GetDirection());
        float distanceToEnd = glm::dot(line->GetEnd() - pos, line->GetDirection());

        // If the end of the line is way behind the player, or the start is way ahead...
        if (distanceToEnd < -recycleDistance || distanceToStart > recycleDistance)
        {
            glm::vec3 newStart, newEnd;

            // Run the exact same orthogonal calculation to snap it to the newest grid gap
            if (lookingAlongX)
            {
                float nearestGapZ = std::round((pos.z - halfSpace) / spaceBetweenBlocks) * spaceBetweenBlocks + halfSpace;
                float dirSign = (forward.x > 0.0f) ? 1.0f : -1.0f;
                newStart = glm::vec3(pos.x - (1000.0f * dirSign), 0.0f, nearestGapZ);
                newEnd = glm::vec3(pos.x + (1000.0f * dirSign), 0.0f, nearestGapZ);
            }
            else
            {
                float nearestGapX = std::round((pos.x - halfSpace) / spaceBetweenBlocks) * spaceBetweenBlocks + halfSpace;
                float dirSign = (forward.z > 0.0f) ? 1.0f : -1.0f;
                newStart = glm::vec3(nearestGapX, 0.0f, pos.z - (1000.0f * dirSign));
                newEnd = glm::vec3(nearestGapX, 0.0f, pos.z + (1000.0f * dirSign));
            }

            line->SetStart(newStart);
            line->SetEnd(newEnd);
        }
    }
}

void Spawner::EditorUpdate()
{
}

void Spawner::Load()
{
}

void Spawner::Unload()
{
}

void Spawner::Start()
{
    GameObject::Start();

    // 1. Define your parameters (Must match your Update loop!)
    //int radius = 2; // A radius of 2 creates a 5x5 grid (25 blocks total)
    //float spaceBetweenBlocks = 10.0f; // Replace with your actual spacing

    // 2. Clear the grid just in case
    blocks.clear();

    // 3. Populate the grid symmetrically around (0,0)
    for (int x = -radius; x <= radius; ++x)
    {
        std::vector<MemoryBlock*> row;
        for (int z = -radius; z <= radius; ++z)
        {
            MemoryBlock* newBlock = new MemoryBlock();

            GetOwningScene()->RegisterGameObject(newBlock, std::to_string((long long)newBlock));

            // Calculate the starting position offset
            glm::vec3 startPos(x * spaceBetweenBlocks,
                0.0f, // Or whatever your floor Y-height is
                z * spaceBetweenBlocks);

            newBlock->SetPosition(startPos);

            // Optional: If your engine requires physics syncing on spawn
            // newBlock->SyncPhysics();

            row.push_back(newBlock);
        }
        blocks.push_back(row);
    }

    // 4. Initialize the tracking variable so the first frame doesn't force a jump
    blockPos = glm::ivec2(0, 0);
}
