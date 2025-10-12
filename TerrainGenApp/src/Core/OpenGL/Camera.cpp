#include "Camera.h"

using namespace Math;

Camera::Camera(float fov, float aspectRatio, float nearClip, float farClip)
	: fov(fov), aspectRatio(aspectRatio), nearClip(nearClip), farClip(farClip), projection(Mat4().Perspective(fov, aspectRatio, nearClip, farClip))
{
	UpdateView();
}

Quat Camera::GetOrientation() const
{
	return Quat(-pitch, -yaw, 0.0f);
}

Vec3 Camera::GetUpVector() const
{
	Quat quat = GetOrientation();
	return quat.Rotate(Vec3(0.0f, 1.0f, 0.0f));
}

Vec3 Camera::GetRightVector() const
{
	Quat quat = GetOrientation();
	return quat.Rotate(Vec3(1.0f, 0.0f, 0.0f));
}

Vec3 Camera::GetForwardVector() const
{
	Quat quat = GetOrientation();
	return quat.Rotate(Vec3(0.0f, 0.0f, -1.0f));
}

void Camera::SetViewportSize(float width, float height)
{
	this->width = (unsigned int)width;
	this->height = (unsigned int)height;

	UpdateProjection();
}

void Camera::SetFocus(const Vec3& focalPoint)
{
	this->focalPoint = focalPoint;

	UpdateView();
}

void Camera::Update(float delta)
{

}

void Camera::OnEvent(Event& event)
{
	switch (event.GetType())
	{
		case EventType::MouseButtonDown:
		{
			auto& ev = static_cast<MouseButtonDownEvent&>(event);
			OnMouseDown(ev);
			break;
		}
		case EventType::MouseButtonUp:
		{
			auto& ev = static_cast<MouseButtonUpEvent&>(event);
			OnMouseUp(ev);
			break;
		}
		case EventType::MouseScroll:
		{
			auto& ev = static_cast<MouseScrollEvent&>(event);
			OnMouseScroll(ev);
			break;
		}
		case EventType::KeyUp:
		{
			auto& ev = static_cast<KeyDownEvent&>(event);
			OnKeyDown(ev);
			break;
		}
		case EventType::KeyDown:
		{
			auto& ev = static_cast<KeyUpEvent&>(event);
			OnKeyUp(ev);
			break;
		}
		case EventType::MouseMove:
		{
			auto& ev = static_cast<MouseMoveEvent&>(event);
			OnMouseMove(ev);
			break;
		}
	}
}

void Camera::UpdateProjection()
{
	aspectRatio = (float)width / (float)height;
	projection = Mat4().Perspective(fov, aspectRatio, nearClip, farClip);
}

void Camera::UpdateView()
{
	position = CalculatePosition();

	Quat orientation = GetOrientation();
	view = Mat4().Translate(position) * orientation.ToMat4();
	view = view.Inverse();
}

void Camera::Pan(const Vec2& delta)
{
	auto [x, y] = PanSpeed();
	focalPoint += -GetRightVector() * delta.x * x * distance;
	focalPoint += -GetUpVector() * delta.y * y * distance;
}

void Camera::Rotate(const Vec2& delta)
{
	float yawSign = GetUpVector().y < 0 ? -1.0f : 1.0f;
	yaw += yawSign * delta.x * turnSpeed;
	pitch += delta.y * turnSpeed;
}

void Camera::Zoom(float delta)
{
	distance -= delta * ZoomSpeed();
	if (distance < 1.0f)
	{
		focalPoint += GetForwardVector();
		distance = 1.0f;
	}
}

std::pair<float, float> Camera::PanSpeed() const
{
	float x = std::min(width / 1000.0f, 2.4f);
	float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

	float y = std::min(height / 1000.0f, 2.4f);
	float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

	return { xFactor, yFactor };
}

float Camera::ZoomSpeed() const
{
	float distance = this->distance * 0.2f;
	distance = std::max(distance, 0.0f);
	float speed = distance * distance;
	speed = std::min(speed, 100.0f);
	return speed;
}

Vec3 Camera::CalculatePosition() const
{
	return focalPoint - GetForwardVector() * distance;
}

void Camera::OnMouseDown(MouseButtonDownEvent& event)
{
	switch (event.button)
	{
		case 1: // Left Button
			leftMouse = true;
			break;
		case 2: // Middle Button
			middleMouse = true;
			break;
		case 3: // Right Button
			rightMouse = true;
			break;
			// 4 Side button
			// 5 Side button
		default:
			break;
	}

	event.handled = true;
}

void Camera::OnMouseUp(MouseButtonUpEvent& event)
{
	switch (event.button)
	{
		case 1: // Left Button
			leftMouse = false;
			break;
		case 2: // Middle Button
			middleMouse = false;
			break;
		case 3: // Right Button
			rightMouse = false;
			break;
			// 4 Side button
			// 5 Side button
		default:
			break;
	}

	event.handled = true;
}

void Camera::OnMouseScroll(MouseScrollEvent& event)
{
	Zoom(event.yScroll);

	event.handled = true;
}

void Camera::OnMouseMove(MouseMoveEvent& event)
{
	const Vec2& mouse = { (float)event.x, (float)event.y };
	Vec2 mouseDelta = (mouse - lastMousePos) * 0.009f;
	lastMousePos = mouse;
	if (middleMouse)
	{
		Pan(mouseDelta);
	}
	else if (rightMouse && leftMouse)
	{
		Zoom(mouseDelta.y);
	}
	else if (rightMouse)
	{
		Rotate(mouseDelta);
	}

	event.handled = true;
}

void Camera::OnKeyDown(KeyDownEvent& event)
{
}

void Camera::OnKeyUp(KeyUpEvent& event)
{
}
