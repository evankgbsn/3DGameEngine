#include "Player.h"

#include "GameEngine/GameObject/Component/CameraComponent.h"
#include "GameEngine/GameObject/Component/GraphicsObjectTexturedAnimatedLit.h"
#include "GameEngine/GameObject/Component/RigidBodyComponent.h"
#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/Input/InputManager.h"
#include "GameEngine/GameObject/Component/GraphicsObjectColored.h"
#include "GameEngine/Renderer/Window/WindowManager.h"
#include "Bullet.h"
#include "GameEngine/Scene/SceneManager.h"
#include "GameEngine/Scene/Scene.h"

Player::Player() :
    GameObject("Player"),
    cam(nullptr),
    body(nullptr),
    graphics(nullptr),
    move(nullptr)
{
    RegisterGameObjectClassType<Player>(this);

    move = new std::function<void(int)>([this](int keyCode)
        {
            if (glm::length(body->GetVelocity()) > 10.0f)
            {
                return;
            }

            float force = 0.0f;

            glm::vec3 moveVector;

            switch (keyCode)
            {
            case KEY_W:
                moveVector = graphics->GetTransform()[2];
                force = 5.0f;
                break;
            case KEY_A:
                moveVector = graphics->GetTransform()[0];
                force = 5.0f;
                break;
            case KEY_S:
                moveVector = -graphics->GetTransform()[2];
                force = 5.0f;
                break;
            case KEY_D:
                moveVector = -graphics->GetTransform()[0];
                force = 5.0f;
                break;
            case KEY_SPACE:
                moveVector = graphics->GetTransform()[1];
                graphics->SetClip(1);
                graphics->SetSpeed(1.0f);
                force = 2000.0f;
                break;
            }

            body->AddForce(moveVector * force);
        });

    look = new std::function<void(const glm::vec2&) > ([this](const glm::vec2& cursorPos)
        {
            Window* window = WindowManager::GetWindow("Engine");
            static glm::vec2 prevPos;

            float xspeed = 0.0005f;
            float yspeed = 0.0005f;
            if (cursorPos.x != prevPos.x)
            {
                cam->Rotate(glm::vec3(0.0f, 1.0f, 0.0f), xspeed * (float)-(cursorPos.x - prevPos.x));
            }
            if (cursorPos.y != prevPos.y)
            {
                cam->Rotate(cam->GetRightVector(), yspeed * (float)-(cursorPos.y - prevPos.y));
            }

            prevPos = cursorPos;
        });


    shoot = new std::function<void(int)>([this](int keyCode)
        {
            Bullet* bullet = new Bullet();

            Scene* scene = SceneManager::GetLoadedScene("FPSTest");

            static unsigned int bulletId = 0;
            bullet->SetDirection(cam->GetForwardVector());
            bullet->SetOrigin(cam->GetPosition());

            scene->RegisterGameObject(bullet, "Bullet-" + std::to_string(bulletId++));

        });
}

Player::~Player()
{
    delete shoot;
    delete look;
    delete move;
}

void Player::SetPosition(const glm::vec3& newPosition)
{
    graphics->SetPosition(newPosition);
    sphere->SetPosition(newPosition);
    body->SetPosition(newPosition);
}

void Player::SetRotation(const glm::mat4& newRotation)
{
    graphics->SetRotation(newRotation);
    sphere->SetRotation(newRotation);
}

glm::vec3 Player::GetPosition() const
{
    return graphics->GetPosition();
}

glm::mat4 Player::GetRotation() const
{
    return graphics->GetRotation();
}

glm::mat4 Player::GetTransform() const
{
    return graphics->GetTransform();
}

void Player::Start()
{
    cam->SetActive();
    InputManager::DisableCursor("Engine");
}

void Player::End()
{
    InputManager::EnableCursor("Engine");
}

void Player::Serialize()
{
    GameObject::Serialize();
}

void Player::Deserialize()
{
    GameObject::Deserialize();
}

void Player::Initialize()
{
    graphics = new GraphicsObjectTexturedAnimatedLit(ModelManager::LoadModel("Woman", "Assets/Model/Woman.gltf"), TextureManager::LoadTexture("Assets/Texture/Woman.png", "Woman"), TextureManager::LoadTexture("Assets/Texture/Woman.png","Woman"));
    graphics->SetClip(0);
    graphics->SetPosition({ 10.0f, 20.0f, 0.0f});
    graphics->SetSpeed(1.0f);
    graphics->SetShine(0.0f);
    AddComponent(graphics, "Graphics");

    sphere = new GraphicsObjectColored(ModelManager::GetModel("Sphere"), { 1.0f, 1.0f, 1.0f, 1.0f });
    sphere->Disable();
    AddComponent(sphere, "Sphere");

    cam = new CameraComponent("Player");
    AddComponent(cam, "Camera");

    body = new RigidBodyComponent(RigidBodyComponent::Type::DYNAMIC, this, sphere->GetModel());
    body->LockAngularMotionOnAxisX();
    body->LockAngularMotionOnAxisZ();
    body->LockAngularMotionOnAxisY();
    body->SyncPhysics();
    AddComponent(body, "RigidBody");

    InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_W, move, "MoveForward");
    InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_S, move, "MoveBackward");
    InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_A, move, "MoveLeft");
    InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_D, move, "MoveRight");
    InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_SPACE, move, "Jump");

    InputManager::RegisterCallbackForMouseButtonState(KEY_PRESS, MOUSE_BUTTON_1, shoot, "Shoot");

    
}

void Player::Terminate()
{
    InputManager::DeregisterCallbackForMouseButtonState(KEY_PRESS, MOUSE_BUTTON_1, "Shoot");

    InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_W, "MoveForward");
    InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_S, "MoveBackward");
    InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_A, "MoveLeft");
    InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_D, "MoveRight");
    InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_SPACE, "Jump");

    delete cam;
    delete graphics;
    delete body;
}

void Player::GameUpdate()
{

    glm::vec3 playerForward = graphics->GetRotation()[2];
    glm::vec3 playerUp = graphics->GetRotation()[1];
    glm::vec3 playerPosition = graphics->GetPosition();
    glm::vec3 camPos = playerPosition + (playerUp * 5.0f) + (playerForward * -4.0f);
    glm::vec3 camTarget = camPos + playerForward;

    sphere->SetPosition(graphics->GetPosition());
    sphere->SetRotation(graphics->GetRotation());
    body->SyncPhysics();
    InputManager::WhenCursorMoved(*look);

    glm::vec3 camPosition = cam->GetPosition();
    glm::vec3 camTargetPosition = cam->GetTarget();

    glm::vec3 motionVector = camPos - camPosition;
    
    glm::vec3 camRight = cam->GetRightVector();
    glm::vec3 newForward = glm::normalize(glm::cross(camRight, glm::vec3(0.0f, 1.0f, 0.0f)));
    glm::vec3 newUp = glm::normalize(glm::cross(newForward, camRight));

    graphics->SetRotation(glm::mat4(glm::vec4(-camRight, 0.0f), glm::vec4(newUp, 0.0f), glm::vec4(-newForward, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));

    cam->SetPosition(camPos);
    cam->SetTarget(camTargetPosition + motionVector);

    float speed = glm::length(body->GetVelocity());

    static float idleSet = false;
    static float runSet = false;
    
    if (speed < 1.0f && !idleSet)
    {
        graphics->SetClip(4);
        graphics->SetSpeed(1.0f);

        runSet = false;
        idleSet = true;
    }

    if (speed > 1.0f && abs(body->GetVelocity().y) < 1.0f && !runSet)
    {
        graphics->SetClip(0);
        graphics->SetSpeed(1.0f);

        runSet = true;
        idleSet = false;
    }
}

void Player::EditorUpdate()
{
}

void Player::Load()
{
    if (!ModelManager::ModelLoaded("Woman"))
    {
        ModelManager::LoadModel("Woman", "Assets/Model/Woman.gltf");
    }

    if (!TextureManager::TextureLoaded("Woman"))
    {
        TextureManager::LoadTexture("Assets/Texture/Woman.png", "Woman");
    }
}

void Player::Unload()
{
}

bool Player::Hovered() const
{
    return body->Hovered();
}
