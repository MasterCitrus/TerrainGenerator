#pragma once

#include "types/math/Mat3.h"
#include "types/math/Mat4.h"
#include "types/math/Quat.h"
#include "types/math/Utils.h"
#include "types/math/Vec2.h"
#include "../Events/Event.h"

#include <utility>

using namespace Math;

class Camera
{
public:
	Camera() = default;
	Camera(float fov, float aspectRatio, float nearClip, float farClip);

	const Mat4& GetViewMatrix() const { return view; }
	Mat4 GetProjectionMatrix() const { return projection; }

	Vec3& GetPosition() { return position; }
	Quat GetOrientation() const;

	Vec3 GetUpVector() const;
	Vec3 GetRightVector() const;
	Vec3 GetForwardVector() const;

	float GetAspectRatio() const { return aspectRatio; }

	void SetViewportSize(float width, float height);
	void SetFocus(const Vec3& focalPoint);

	void Update(float delta);
	void OnEvent(Event& event);

	Vec2 GetLastMousePos() const { return lastMousePos; }

private:
	void UpdateProjection();
	void UpdateView();

	void Pan(const Vec2& delta);
	void Rotate(const Vec2& delta);
	void Zoom(float delta);

	std::pair<float, float> PanSpeed() const;
	float ZoomSpeed() const;

	Vec3 CalculatePosition() const;

	void OnMouseDown(MouseButtonDownEvent& event);
	void OnMouseUp(MouseButtonUpEvent& event);
	void OnMouseScroll(MouseScrollEvent& event);
	void OnMouseMove(MouseMoveEvent& event);
	void OnKeyDown(KeyDownEvent& event);
	void OnKeyUp(KeyUpEvent& event);

private:
	Mat4 projection = Mat4();
	Mat4 view;

	Vec3 focalPoint = { 0.0f, 0.0f, 0.0f };
	Vec3 position = { 0.0f, 2.0f, 10.0f };

	Vec2 lastMousePos = { 0.0f, 0.0f };

	float pitch = Deg2Rad(30.0f), yaw = 0.0f;
	float distance = 10.0f;
	float fov = 45.0f;
	float aspectRatio = 1.778f;
	float nearClip = 0.1f;
	float farClip = 1000.0f;

	float turnSpeed = 0.25f;
	float cameraSpeed = 10.0f;

	unsigned int width = 1280, height = 720;

	bool rightMouse = false;
	bool leftMouse = false;
	bool middleMouse = false;
};