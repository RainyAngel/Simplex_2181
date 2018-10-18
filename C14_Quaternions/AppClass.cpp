#include "AppClass.h"
void Application::InitVariables(void)
{
	//Set the position and target of the camera
	//(I'm at [0,0,10], looking at [0,0,0] and up is the positive Y axis)
	m_pCameraMngr->SetPositionTargetAndUpward(AXIS_Z * 10.0f, ZERO_V3, AXIS_Y);

	//initializing the model
	m_pModel = new Simplex::Model();

	//Load a model
	m_pModel->Load("Minecraft\\Steve.obj");

	m_pMesh = new MyMesh();
	m_pMesh->GenerateCube(7.0f, C_RED);
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();

	//Get a timer
	static uint uClock = m_pSystem->GenClock();
	float fTimer = m_pSystem->GetTimeSinceStart(uClock);
	float fDeltaTime = m_pSystem->GetDeltaTime(uClock);

#pragma region SLERP
	if (false)
	{
		quaternion q1;
		//quaternion q2 = glm::angleAxis(glm::radians(359.9f), vector3(0.0f, 0.0f, 1.0f));
		//quaternion q2 = glm::angleAxis(glm::radians(180.0f), vector3(0.0f, 0.0f, 1.0f));
		//quaternion q2 = glm::angleAxis(glm::radians(360.0f), vector3(0.0f, 0.0f, 1.0f)); --> use this instead
		//the above doesn't work because of rounding errors
		quaternion q2 = glm::angleAxis(glm::radians(359.9f), vector3(0.0f, 0.0f, 1.0f));

		//5 seconds
		//float fPercentage = MapValue(fTimer, 0.0f, 5.0f, 0.0f, 1.0f);
		//2 seconds
		float fPercentage = MapValue(fTimer, 0.0f, 2.0f, 0.0f, 1.0f);
		//give 2 orientations and give the lerp of the two orientations
		quaternion qSLERP = glm::mix(q1, q2, fPercentage);
		m_m4Steve = glm::toMat4(qSLERP);
	}
#pragma endregion
#pragma region translate vector orientation into a matrix
	if (false)
	{
		matrix4 m4OrientX = glm::rotate(IDENTITY_M4, glm::radians(m_v3Orientation.x), vector3(1.0f, 0.0f, 0.0f));
		matrix4 m4OrientY = glm::rotate(IDENTITY_M4, glm::radians(m_v3Orientation.y), vector3(0.0f, 1.0f, 0.0f));
		matrix4 m4OrientZ = glm::rotate(IDENTITY_M4, glm::radians(m_v3Orientation.z), vector3(0.0f, 0.0f, 1.0f));

		matrix4 m4Orientation = m4OrientX * m4OrientY * m4OrientZ;
		m_m4Steve = glm::toMat4(m_qOrientation);
	}
#pragma endregion
#pragma region orientation using quaternions 
	if (true)
	{
		m_m4Steve = glm::toMat4(m_qOrientation);
	}
#pragma endregion
	
	//Attach the model matrix that takes me from the world coordinate system
	m_pModel->SetModelMatrix(m_m4Steve);

	//Send the model to render list
	m_pModel->AddToRenderList();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();
	
	matrix4 m4Model = IDENTITY_M4; //glm::translate(m_v3Orientation);

	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();

	matrix4 m4View = m_pCameraMngr->GetViewMatrix();

	//orthographic view
	//m4Projection = glm::ortho(-25.0f, 25.0f, -5.0f, 5.0f, 0.01f, 100.0f);

	//perspective view
	//float fFOV = 45.0f + (m_v3Orientation.x * 0.01f); //how wide is my field of view (usually 90.0f degrees)
	float fFOV = 90.0f;
	float fAspect = static_cast<float>(m_pSystem->GetWindowWidth()) / static_cast<float>(m_pSystem->GetWindowHeight());
	float fNear = 0.01f;
	float fFar = 20.0f;

	m4Projection = glm::perspective(fFOV, fAspect, fNear, fFar);

	vector3 v3Position = vector3(0.0f, 0.0f, 5.0f);
	vector3 v3Target;
	vector3 v3Up = vector3(0.0f, 1.0f, 0.0f);

	//view matrix is where is the camera is facing
	m4View = glm::lookAt(v3Position, v3Target, v3Up);


	//need model matrix, view matrix, and projection matrix 
	//m_pMesh->Render(m4Projection, m4View, m4Model);

	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();

	//matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();
	//matrix4 m4View = m_pCameraMngr->GetViewMatrix();

	/*float fovy = m_fFovy; // field of view
	//float aspect = 1; //aspect ratio
	//float aspect = 1080.0f / 720.0f; //aspect ratio
	float aspect = m_pSystem->GetWindowWidth() / m_pSystem->GetWindowHeight(); //aspect ratio
	float zNear = 1.0f; //near clipping plane
	float zFar = 1000.0f; //far clipping plane

	m4Projection = glm::perspective(fovy, aspect, zNear, zFar);*/

	//send back the matrix you calculated 
	m_pCameraMngr->SetProjectionMatrix(m4Projection);
	m_pCameraMngr->SetViewMatrix(m4View);

	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	//release model
	SafeDelete(m_pModel);

	SafeDelete(m_pMesh);

	//release GUI
	ShutdownGUI();
}
