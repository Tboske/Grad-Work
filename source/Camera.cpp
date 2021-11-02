#include "pch.h"
#include "Camera.h"
#include "SDL.h"
//#include "SDL_surface.h"
#include <cmath>

Camera::Camera(const Elite::FPoint3& pos, const Elite::FVector3& forward, float aspectRatio, float fov, float nearPlane, float farPlane)
	: m_AspectRatio{ aspectRatio }
	, m_Fov{ std::tan(Elite::ToRadians(fov)/2) }
	, m_RotationSensitivity{ 15 }
	, m_MovementSensitivity{ 10 }
	, m_ProjectionMatrix{ Elite::FMatrix4() }
{ 
	m_ProjectionMatrix[0][0] = 1 / (aspectRatio * m_Fov);
	m_ProjectionMatrix[1][1] = 1 / m_Fov;
	m_ProjectionMatrix[2][2] = (farPlane / (farPlane - nearPlane));
	m_ProjectionMatrix[3][2] = (-(farPlane * nearPlane) / (farPlane - nearPlane));
	m_ProjectionMatrix[2][3] = 1;

	// construct ONB
	ReconstructONB(forward);
	m_ONB[3] = Elite::FVector3{ pos };
	m_ONB[3][3] = 1.f;

	// invert position.z
	m_ONB[3][2] = -m_ONB[3][2];
}
 
Camera::~Camera()
{
}

Elite::FMatrix4 Camera::GetONB() const
{
	return Elite::Inverse(m_ONB);
}

const Elite::FMatrix4& Camera::GetProjectionMatrix() const
{
	return m_ProjectionMatrix;
}

void Camera::CheckInputs(float dTime)
{
	int x{ 0 }, y{ 0 };
	Uint32 mouseState = SDL_GetMouseState(&x, &y);
	float dX = ((float)x - m_LastX) / 1000;
	float dY = ((float)y - m_LastY) / 1000;

	m_LastX = x;
	m_LastY = y;

	switch (mouseState)
	{
	case 5: // left + right click
		m_ONB[3].xyz = m_ONB[3].xyz + (worldUp * -dY);
		m_ONB[3].xyz = m_ONB[3].xyz + (m_Forward * dX);
		break;
	case 4: // right click 
		// left - right rotation
		ReconstructONB( Elite::MakeRotationY(-dX * m_RotationSensitivity) * m_Forward );

		// up - down movement
		ReconstructONB( Elite::MakeRotation(-dY * m_RotationSensitivity, Elite::GetNormalized(Elite::Cross(worldUp, m_Forward))) * m_Forward );

		break;
	case 1: // left click
		Elite::FVector3 axis = Elite::MakeRotationY(-dX * m_RotationSensitivity) * m_Forward;
		ReconstructONB(axis);

		Elite::Normalize(axis);
		m_ONB[3].xyz = m_ONB[3].xyz + (axis * dY);
		break;
	default:
		break;
	}

	const Uint8* keyState = SDL_GetKeyboardState(NULL);
	if (keyState[SDL_SCANCODE_A])
		m_ONB[3].xyz = m_ONB[3].xyz + (-Elite::GetNormalized(m_ONB[0].xyz) * dTime * m_MovementSensitivity);
	
	else if (keyState[SDL_SCANCODE_D])
		m_ONB[3].xyz = m_ONB[3].xyz + (Elite::GetNormalized(m_ONB[0].xyz) * dTime * m_MovementSensitivity);


	if (keyState[SDL_SCANCODE_W])
		m_ONB[3].xyz = m_ONB[3].xyz + (Elite::GetNormalized(m_ONB[2].xyz) * dTime * m_MovementSensitivity);
	
	else if (keyState[SDL_SCANCODE_S])
		m_ONB[3].xyz = m_ONB[3].xyz + (-Elite::GetNormalized(m_ONB[2].xyz) * dTime * m_MovementSensitivity);
	

	if (keyState[SDL_SCANCODE_E])
		m_ONB[3].xyz = m_ONB[3].xyz + (worldUp * dTime * m_MovementSensitivity);
	
	else if (keyState[SDL_SCANCODE_Q])
		m_ONB[3].xyz = m_ONB[3].xyz + (-worldUp * dTime * m_MovementSensitivity);
}

void Camera::ReconstructONB(const Elite::FVector3& forward)
{
	m_Forward = Elite::GetNormalized(forward);
	// forward
	m_ONB[2] = { -forward.x, -forward.y, forward.z };

	// right
	m_ONB[0] = Elite::GetNormalized(Elite::Cross(worldUp, m_ONB[2].xyz));

	// up
	m_ONB[1] = Elite::GetNormalized(Elite::Cross(m_ONB[2].xyz, m_ONB[0].xyz));
}
