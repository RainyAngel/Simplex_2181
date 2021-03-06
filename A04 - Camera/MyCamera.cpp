#include "MyCamera.h"
using namespace Simplex;

//Accessors
void Simplex::MyCamera::SetPosition(vector3 a_v3Position) { m_v3Position = a_v3Position; }
vector3 Simplex::MyCamera::GetPosition(void) { return m_v3Position; }
void Simplex::MyCamera::SetTarget(vector3 a_v3Target) { m_v3Target = a_v3Target; }
vector3 Simplex::MyCamera::GetTarget(void) { return m_v3Target; }
void Simplex::MyCamera::SetAbove(vector3 a_v3Above) { m_v3Above = a_v3Above; }
vector3 Simplex::MyCamera::GetAbove(void) { return m_v3Above; }
void Simplex::MyCamera::SetPerspective(bool a_bPerspective) { m_bPerspective = a_bPerspective; }
void Simplex::MyCamera::SetFOV(float a_fFOV) { m_fFOV = a_fFOV; }
void Simplex::MyCamera::SetResolution(vector2 a_v2Resolution) { m_v2Resolution = a_v2Resolution; }
void Simplex::MyCamera::SetNearFar(vector2 a_v2NearFar) { m_v2NearFar = a_v2NearFar; }
void Simplex::MyCamera::SetHorizontalPlanes(vector2 a_v2Horizontal) { m_v2Horizontal = a_v2Horizontal; }
void Simplex::MyCamera::SetVerticalPlanes(vector2 a_v2Vertical) { m_v2Vertical = a_v2Vertical; }
matrix4 Simplex::MyCamera::GetProjectionMatrix(void) { return m_m4Projection; }
matrix4 Simplex::MyCamera::GetViewMatrix(void) { CalculateViewMatrix(); return m_m4View; }

Simplex::MyCamera::MyCamera()
{
	Init(); //Init the object with default values
}

Simplex::MyCamera::MyCamera(vector3 a_v3Position, vector3 a_v3Target, vector3 a_v3Upward)
{
	Init(); //Initialize the object
	SetPositionTargetAndUpward(a_v3Position, a_v3Target, a_v3Upward); //set the position, target and upward
}

Simplex::MyCamera::MyCamera(MyCamera const& other)
{
	m_v3Position = other.m_v3Position;
	m_v3Target = other.m_v3Target;
	m_v3Above = other.m_v3Above;

	m_bPerspective = other.m_bPerspective;

	m_fFOV = other.m_fFOV;

	m_v2Resolution = other.m_v2Resolution;
	m_v2NearFar = other.m_v2NearFar;

	m_v2Horizontal = other.m_v2Horizontal;
	m_v2Vertical = other.m_v2Vertical;

	m_m4View = other.m_m4View;
	m_m4Projection = other.m_m4Projection;
}

MyCamera& Simplex::MyCamera::operator=(MyCamera const& other)
{
	if (this != &other)
	{
		Release();
		SetPositionTargetAndUpward(other.m_v3Position, other.m_v3Target, other.m_v3Above);
		MyCamera temp(other);
		Swap(temp);
	}
	return *this;
}

void Simplex::MyCamera::Init(void)
{
	ResetCamera();
	CalculateProjectionMatrix();
	CalculateViewMatrix();
	//No pointers to initialize here
}

void Simplex::MyCamera::Release(void)
{
	//No pointers to deallocate
}

void Simplex::MyCamera::Swap(MyCamera & other)
{
	std::swap(m_v3Position, other.m_v3Position);
	std::swap(m_v3Target, other.m_v3Target);
	std::swap(m_v3Above, other.m_v3Above);
	
	std::swap(m_bPerspective, other.m_bPerspective);

	std::swap(m_fFOV, other.m_fFOV);

	std::swap(m_v2Resolution, other.m_v2Resolution);
	std::swap(m_v2NearFar, other.m_v2NearFar);

	std::swap(m_v2Horizontal, other.m_v2Horizontal);
	std::swap(m_v2Vertical, other.m_v2Vertical);

	std::swap(m_m4View, other.m_m4View);
	std::swap(m_m4Projection, other.m_m4Projection);
}

Simplex::MyCamera::~MyCamera(void)
{
	Release();
}

void Simplex::MyCamera::ResetCamera(void)
{
	m_v3Position = vector3(0.0f, 0.0f, 10.0f); //Where my camera is located
	m_v3Target = vector3(0.0f, 0.0f, 0.0f); //What I'm looking at
	m_v3Above = vector3(0.0f, 1.0f, 0.0f); //What is above the camera

	m_bPerspective = true; //perspective view? False is Orthographic

	m_fFOV = 45.0f; //Field of View

	m_v2Resolution = vector2(1280.0f, 720.0f); //Resolution of the window
	m_v2NearFar = vector2(0.001f, 1000.0f); //Near and Far planes

	m_v2Horizontal = vector2(-5.0f, 5.0f); //Orthographic horizontal projection
	m_v2Vertical = vector2(-5.0f, 5.0f); //Orthographic vertical projection

	CalculateProjectionMatrix();
	CalculateViewMatrix();
}

void Simplex::MyCamera::SetPositionTargetAndUpward(vector3 a_v3Position, vector3 a_v3Target, vector3 a_v3Upward)
{
	m_v3Position = a_v3Position;
	m_v3Target = a_v3Target;

	m_v3Above = a_v3Position + glm::normalize(a_v3Upward);
	
	//Calculate the Matrix
	CalculateProjectionMatrix();
}

void Simplex::MyCamera::CalculateViewMatrix(void)
{
	//Calculate the look at most of your assignment will be reflected in this method
	m_m4View = glm::lookAt(m_v3Position, m_v3Target, glm::normalize(m_v3Above - m_v3Position)); //position, target, upward
}

void Simplex::MyCamera::CalculateProjectionMatrix(void)
{
	float fRatio = m_v2Resolution.x / m_v2Resolution.y;
	if (m_bPerspective) //perspective
	{
		m_m4Projection = glm::perspective(m_fFOV, fRatio, m_v2NearFar.x, m_v2NearFar.y);
	}
	else //Orthographic
	{
		m_m4Projection = glm::ortho(m_v2Horizontal.x * fRatio, m_v2Horizontal.y * fRatio, //horizontal
			m_v2Vertical.x, m_v2Vertical.y, //vertical
			m_v2NearFar.x, m_v2NearFar.y); //near and far
	}
}

void MyCamera::MoveForward(float a_fDistance)
{
	//want to go from position to target (aka the direction my camera is facing)
	vector3 direction = (m_v3Target - m_v3Position);

	//normalize the vector
	direction = glm::normalize(direction);

	//multiply by distance to get how far in the direction to go
	direction = direction * a_fDistance;

	//apply the direction vector
	m_v3Position += direction;
	m_v3Target += direction;
	m_v3Above += direction;
}

void MyCamera::MoveVertical(float a_fDistance)
{
	//want to go from position to the point above
	vector3 direction = (m_v3Above - m_v3Position);

	//lather, rinse, repeat
	direction = glm::normalize(direction);

	direction = direction * a_fDistance;

	m_v3Position += direction;
	m_v3Target += direction;
	m_v3Above += direction;
} 

void MyCamera::MoveSideways(float a_fDistance)
{
	vector3 direction = (m_v3Target - m_v3Position);
	vector3 yDirection = (m_v3Above - m_v3Position);

	direction = glm::normalize(direction);
	yDirection = glm::normalize(yDirection); 

	//rotate the forward vector around the y vector
	direction = glm::rotate(direction, glm::radians(90.0f), yDirection) * a_fDistance; 

	m_v3Position += direction;
	m_v3Target += direction;
	m_v3Above += direction;
}

void MyCamera::Rotate(uint x, uint y)
{
#pragma region Getting Mouse Position
	//mouse pos
	UINT	MouseX, MouseY;		

	//center of screen
	UINT	CenterX, CenterY;	

	//set pos of pointer to center screen 
	CenterX = x;
	CenterY = y;

	//Calculate the position of the mouse and store it
	POINT pt;
	GetCursorPos(&pt);
	MouseX = pt.x;
	MouseY = pt.y;  
#pragma endregion Getting Mouse Position

	//the angles of rotation
	int xTheta = -(((int)MouseX - (int)CenterX) * 0.1f);
	int yTheta = ((int)MouseY - (int)CenterY) * 0.1f;

	//check maximum rotation speed of xTheta
	if (xTheta > 5) {
		xTheta = 5;
	}
	//check minimum rotation speed of xTheta
	else if (xTheta < -5) {
		xTheta = -5;
	}
	//check maximum rotation speed of yTheta
	if (yTheta > 5) {
		yTheta = 5;
	}
	//check minimum rotation speed of yTheta
	else if (yTheta < -5) {
		yTheta = -5;
	}

	//the directions I need to get my actual rotation direction
	vector3 zDirection = (m_v3Target - m_v3Position);
	vector3 yDirection = (m_v3Above - m_v3Position);

	zDirection = glm::normalize(zDirection);
	yDirection = glm::normalize(yDirection);

	//the direction I am going to rotate vertically around
	vector3 xDirection = glm::rotate(zDirection, glm::radians(90.0f), yDirection);

	//what to rotate
	vector3 forwardVector = (m_v3Target - m_v3Position);
	vector3 upVector = (m_v3Above - m_v3Position);

	forwardVector = glm::normalize(forwardVector);
	upVector = glm::normalize(upVector);

	//rotate x and y
	forwardVector = glm::rotate(forwardVector, glm::radians((float)xTheta), AXIS_Y);
	upVector = glm::rotate(upVector, glm::radians((float)yTheta), xDirection);

	//now rotate m_v3Target around the relative "right"
	m_v3Target = m_v3Position + glm::rotate(upVector, glm::radians(90.0f), xDirection);

	//set the forward vector after rotating vertically
	forwardVector = (m_v3Target - m_v3Position);
	forwardVector = glm::normalize(forwardVector);

	//rotate it with the new vertical rotation accounted for
	forwardVector = glm::rotate(forwardVector, glm::radians((float)xTheta), AXIS_Y);
	
	//apply the rotation
	m_v3Target = m_v3Position + forwardVector;

	//rotate up vector with everything
	upVector = glm::rotate(upVector, glm::radians((float)xTheta), AXIS_Y);

	//rotate above accordingly
	m_v3Above = m_v3Position + upVector;
}



