#include "Camera.h"

#include "../Window/Window.h"
#include "../Window/WindowManager.h"
#include "CameraManager.h"
#include "../../Utils/Logger.h"


#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp >

Camera::~Camera()
{
	if (window != nullptr)
	{
		window->DeregisterCallbackForWindowResize("Camera:" + name);
	}

	delete windowResizeCallback;
}

void Camera::Rotate(const glm::vec3& axis, const float& angle)
{	
	target = position + glm::rotate(GetForwardVector(), angle, axis) * 100000.0f;
	UpdateView();
}

void Camera::Translate(const glm::vec3& translation)
{
	target += translation;
	position += translation;
	UpdateView();
}

glm::vec3 Camera::GetForwardVector() const
{
	return glm::normalize(target - position);
}

glm::vec3 Camera::GetRightVector() const
{
	return -glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), GetForwardVector()));
}

glm::vec3 Camera::GetUpVector() const
{
	return glm::normalize(glm::cross(GetForwardVector(), -GetRightVector()));
}

const float& Camera::GetFOV() const
{
	return fov;
}

const float& Camera::GetNear() const
{
	return near;
}

const float& Camera::GetFar() const
{
	return far;
}

const glm::vec3& Camera::GetPosition() const
{
	return position;
}

const glm::vec3& Camera::GetTarget() const
{
	return target;
}

const glm::mat4& Camera::GetView() const
{
	return view;
}

const glm::mat4& Camera::GetProjection() const
{
	return projection;
}

const Camera::Type& Camera::GetType() const
{
	return type;
}

void Camera::SetPosition(const glm::vec3& p)
{
	position = p;
	UpdateView();
}

void Camera::SetTarget(const glm::vec3& newTarget)
{
	target = newTarget;
	UpdateView();
}

void Camera::SetView(const glm::mat4& newView)
{
	view = newView;
}

void Camera::SetProjection(const glm::mat4& newProjection)
{
	projection = newProjection;
}

void Camera::SetFOV(const float& newFOV)
{
	fov = newFOV;
	UpdateProjection();
}

void Camera::SetAspectRatio(const float& newAspectRatio)
{
	aspect = newAspectRatio;
	UpdateProjection();
}

void Camera::SetNear(const float& newNear)
{
	near = newNear;
	UpdateProjection();
}

void Camera::SetFar(const float& newFar)
{
	far = newFar;
	UpdateProjection();
}

void Camera::SetTop(const float& newTop)
{
	top = newTop;
	UpdateProjection();
}

void Camera::SetBottom(const float& newBottom)
{
	bottom = newBottom;
	UpdateProjection();
}

void Camera::SetLeft(const float& newLeft)
{
	left = newLeft;
	UpdateProjection();
}

void Camera::SetRight(const float& newRight)
{
	right = newRight;
	UpdateProjection();
}

void Camera::SetType(const Camera::Type& newType)
{
	type = newType;
}

void Camera::SetWindow(Window* const newWindow)
{
	window = newWindow;

	if (window != nullptr)
	{
		right = static_cast<float>(window->GetWidth());
		top = static_cast<float>(window->GetHeight());
		aspect = right / top;
		UpdateProjection();
	}
}

LineSegment3D Camera::CastLineFromCursorWithActiveCamera(float distance)
{
	// Screen space to world space for object picking.
	Window* window = WindowManager::GetWindow("Engine");
	Camera& cam = CameraManager::GetActiveCamera();
	glm::vec2 cursorPos = window->GetCursorPosition();
	glm::mat4 invPersp = glm::inverse(cam.GetProjection());
	glm::mat4 invView = glm::inverse(cam.GetView());
	glm::mat4 screenToNDC(
		glm::vec4((float)window->GetWidth() / 2.0f, 0.0f, 0.0f, 0.0f),
		glm::vec4(0, -(float)window->GetHeight() / 2.0f, 0.0f, 0.0f),
		glm::vec4(0.0f, 0.0f, 0.5f, 0.0f),
		glm::vec4((float)window->GetWidth() / 2.0f, (float)window->GetHeight() / 2.0f, 0.5f, 1.0f)
	);
	screenToNDC = glm::inverse(screenToNDC);

	glm::vec4 x = glm::vec4(cursorPos.x, cursorPos.y, 1.0f, 1.0f);
	x = screenToNDC * x;
	x = invPersp * x;
	x = invView * x;
	x /= x.w;

	glm::vec3 direction = glm::normalize(glm::vec3(x) - cam.GetPosition());

	return LineSegment3D(cam.GetPosition(), cam.GetPosition() + direction * distance);
}

void Camera::UpdateProjection()
{
	switch (type)
	{
	case Camera::Type::PERSPECTIVE:
		projection = glm::perspective(fov, aspect, near, far);
		break;
	case Camera::Type::ORTHOGRAPHIC:
		projection = glm::ortho(left, right, bottom, top, near, far);
		break;
	default:
		break;
	}
}

void Camera::UpdateView()
{
	view = glm::lookAt(position, target, GetUpVector());
}

Camera::Camera(const Camera::Type& t, Window* const w, const std::string& n) :
	type(t),
	window(w),
	view(glm::mat4(1.0f)),
	projection(glm::mat4(1.0f)),
	near(1.0f),
	far(1000.0f),
	fov(45.0f),
	left(-10.0f),
	right(10.0f),
	top(10.0f),
	bottom(-10.0f),
	position(glm::vec3(0.0f,0.0f,0.0f)),
	target(glm::vec3(0.0f, 0.0f, 1.0f)),
	name(n)
{
	SetWindow(w);
	UpdateProjection();
	UpdateView();

	windowResizeCallback = new std::function<void(unsigned int, unsigned int)>([this](unsigned int, unsigned int)
		{
			SetWindow(window);
		});

	if (window != nullptr)
	{
		window->RegisterCallbackForWindowResize("Camera:" + name, windowResizeCallback);
	}
}
