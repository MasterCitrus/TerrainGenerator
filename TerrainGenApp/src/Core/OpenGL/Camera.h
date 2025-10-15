#pragma once

#include "../Events/Event.h"

#include <glm/vec2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <utility>

namespace OpenGL
{
	class Camera
	{
	public:
		Camera() = default;
		Camera(float fov, float aspectRatio, float nearClip, float farClip);

		const glm::mat4& GetViewMatrix() const { return view; }
		glm::mat4 GetProjectionMatrix() const { return projection; }

		glm::vec3& GetPosition() { return position; }
		glm::quat GetOrientation() const;

		glm::vec3 GetUpVector() const;
		glm::vec3 GetRightVector() const;
		glm::vec3 GetForwardVector() const;

		float GetAspectRatio() const { return aspectRatio; }

		void SetViewportSize(float width, float height);
		void SetFocus(const glm::vec3& focalPoint);

		void Update(float delta);
		void OnEvent(Event& event);

		glm::vec2 GetLastMousePos() const { return lastMousePos; }

	private:
		void UpdateProjection();
		void UpdateView();

		void Pan(const glm::vec2& delta);
		void Rotate(const glm::vec2& delta);
		void Zoom(float delta);

		std::pair<float, float> PanSpeed() const;
		float ZoomSpeed() const;

		glm::vec3 CalculatePosition() const;

		void OnMouseDown(MouseButtonDownEvent& event);
		void OnMouseUp(MouseButtonUpEvent& event);
		void OnMouseScroll(MouseScrollEvent& event);
		void OnMouseMove(MouseMoveEvent& event);
		void OnKeyDown(KeyDownEvent& event);
		void OnKeyUp(KeyUpEvent& event);

	private:
		glm::mat4 projection = glm::mat4(1);
		glm::mat4 view;

		glm::vec3 focalPoint = { 0.0f, 0.0f, 0.0f };
		glm::vec3 position = { 0.0f, 2.0f, 10.0f };

		glm::vec2 lastMousePos = { 0.0f, 0.0f };

		float pitch = glm::radians(30.0f), yaw = 0.0f;
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
}
