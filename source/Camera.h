#pragma once
#include "EMath.h"

const static Elite::FVector3 worldUp{ 0.f, 1.f, 0.f };

class Camera final
{
public:
	Camera(const Elite::FPoint3& position, const Elite::FVector3& forward, float aspectRatio, float fov, float nearPlane, float farPlane);
	~Camera();

	Elite::FMatrix4 GetONB() const;
	const Elite::FMatrix4& GetProjectionMatrix() const;

	void CheckInputs(float deltaTime);
private:
	// input variables
	int m_LastX, m_LastY;
	float m_RotationSensitivity;
	float m_MovementSensitivity;

	// camera variables
	Elite::FVector3 m_Forward;
	Elite::FMatrix4 m_ONB;
	Elite::FMatrix4 m_ProjectionMatrix;
	const float m_AspectRatio = 0.f;
	const float m_Fov = 0.f;

	//functions
	void ReconstructONB(const Elite::FVector3& forward);
};
