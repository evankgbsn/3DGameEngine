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
    radius(10),
    lineDataSpeed(1000.0f),
    forwardSpawnOffset(20.0f),
    hasReachedGrid(false)
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
    if (TimeManager::SecondsSinceStart() > 3.0f)
    {
        CreateInitialMemoryBlocks();
    }

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

    // KEEP THIS: We need the forward vector for the Data Lines later!
    glm::mat4 rotation = runner->GetRotation();
    glm::vec3 forward = glm::normalize(glm::vec3(rotation[2]));

    // 1. Get the player's actual, current grid cell
    glm::ivec2 playerCell = glm::ivec2(
        std::floor(pos.x / spaceBetweenBlocks),
        std::floor(pos.z / spaceBetweenBlocks)
    );

    // 8. Determine the primary viewing axis using the 'forward' vector
    bool lookingAlongX = std::abs(forward.x) > std::abs(forward.z);

    // ADD THIS: Only run the grid checks if the blocks actually exist!
    if (!blocks.empty())
    {
        // 2. Wait until the player reaches the CENTER of the grid
        if (!hasReachedGrid)
        {
            bool reachedCenter = false;

            if (lookingAlongX)
            {
                // Running along X axis
                if (forward.x > 0.0f && playerCell.x >= blockPos.x) reachedCenter = true;
                else if (forward.x < 0.0f && playerCell.x <= blockPos.x) reachedCenter = true;
            }
            else
            {
                // Running along Z axis
                if (forward.z > 0.0f && playerCell.y >= blockPos.y) reachedCenter = true;
                else if (forward.z < 0.0f && playerCell.y <= blockPos.y) reachedCenter = true;
            }

            if (reachedCenter)
            {
                hasReachedGrid = true;

                // Sync the target to their exact cell (accounts for strafing) 
                // so there is zero snapping when the treadmill starts
                blockPos = playerCell;
            }
        }

        // 3. Once activated, run the treadmill logic centered around the player
        if (hasReachedGrid)
        {
            glm::ivec2 currentBlockPos = playerCell;

            if (blockPos != currentBlockPos)
            {
                int gridWidth = (radius * 2) + 1;
                float totalGridSize = gridWidth * spaceBetweenBlocks;

                for (auto& row : blocks)
                {
                    for (auto& block : row)
                    {
                        if (block == nullptr) continue;

                        glm::vec3 bPos = block->GetPosition();

                        glm::ivec2 bGridPos = glm::ivec2(
                            std::round(bPos.x / spaceBetweenBlocks),
                            std::round(bPos.z / spaceBetweenBlocks)
                        );

                        // 4. Wrap the X axis
                        if (bGridPos.x < currentBlockPos.x - radius)
                        {
                            bPos.x += totalGridSize;
                        }
                        else if (bGridPos.x > currentBlockPos.x + radius)
                        {
                            bPos.x -= totalGridSize;
                        }

                        // 5. Wrap the Z axis
                        if (bGridPos.y < currentBlockPos.y - radius)
                        {
                            bPos.z += totalGridSize;
                        }
                        else if (bGridPos.y > currentBlockPos.y + radius)
                        {
                            bPos.z -= totalGridSize;
                        }

                        // 6. Apply the new position if it changed
                        if (bPos != block->GetPosition())
                        {
                            block->SetPosition(bPos);
                            block->RandomActivate();

                            // NOTE: If you are using PhysX, make sure you update the rigid body directly
                            // block->SyncPhysics(); 
                        }
                    }
                }

                // 7. Update the stored position
                blockPos = currentBlockPos;
            }
        }

        float halfSpace = spaceBetweenBlocks / 2.0f;

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
    
}

void Spawner::CreateInitialMemoryBlocks()
{
    static bool blocksCreated = false;

    if (!blocksCreated)
    {
        HackerRunner* runner = nullptr;
        for (const auto& obj : GetOwningScene()->GetGameObjects())
        {
            if ((runner = dynamic_cast<HackerRunner*>(obj.second)) != nullptr)
            {
                break;
            }
        }

        if (runner == nullptr) return;

        blocksCreated = true;

        glm::vec3 playerPos = runner->GetPosition();

        // Grab the player's forward direction
        glm::mat4 rotation = runner->GetRotation();
        glm::vec3 forward = glm::normalize(glm::vec3(rotation[2]));

        // Calculate a point 'forwardSpawnOffset' blocks ahead of the player
        float projectionDistance = forwardSpawnOffset * spaceBetweenBlocks;
        glm::vec3 projectedPos = playerPos + (forward * projectionDistance);

        // Center the initial grid on that projected position instead of the player
        glm::ivec2 startGridCenter = glm::ivec2(
            std::floor(projectedPos.x / spaceBetweenBlocks),
            std::floor(projectedPos.z / spaceBetweenBlocks)
        );

        blocks.clear();

        for (int x = -radius; x <= radius; ++x)
        {
            std::vector<MemoryBlock*> row;
            for (int z = -radius; z <= radius; ++z)
            {
                MemoryBlock* newBlock = new MemoryBlock();
                GetOwningScene()->RegisterGameObject(newBlock, std::to_string((long long)newBlock));

                glm::vec3 startPos(
                    (startGridCenter.x + x) * spaceBetweenBlocks,
                    0.0f,
                    (startGridCenter.y + z) * spaceBetweenBlocks
                );

                newBlock->SetPosition(startPos);
                newBlock->RandomActivate();

                row.push_back(newBlock);
            }
            blocks.push_back(row);
        }

        blockPos = startGridCenter;
    }
}
