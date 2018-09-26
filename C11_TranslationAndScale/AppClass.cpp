#include "AppClass.h"
void Application::InitVariables(void)
{
	//init the mesh
	for (int i = 0; i < 46; i++)
	{
		m_pMesh[i] = new MyMesh();
		m_pMesh[i]->GenerateCube(1.0f, C_BLACK);
	}

	//x starts at -5 and ends at 5
	//y starts at 4 and ends at -4 
	float xVal = -5.0f;
	float yVal = 4.0f;

	//loop through the space invader array 
	for (int x = 0; x < 11; x++)
	{
		//restore yVal
		yVal = 4.0f;

		for (int y = 0; y < 8; y++)
		{
			//if there is a value of 1, save a certain x and y value 
			if (spaceInvader[y][x] == 1) {
				xValues.push_back(xVal);
				yValues.push_back(yVal);
			}
			//deduct yVal every iteration of the loop
			yVal -= 1.0f;
		}
		//add to xVal every iteration of the loop
		xVal += 1.0f;
	}
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	matrix4 m4View = m_pCameraMngr->GetViewMatrix();
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();
	
	matrix4 m4Scale = glm::scale(IDENTITY_M4, vector3(1.0f,1.0f,1.0f));

	//declare all variables outside of loop
	matrix4 m4Model;
	matrix4 m4Translate;

	//to move spaceInvader
	xMove += 0.01f;
	yMove += yIncrement; 

	if (yMove >= 3.0f) { 
		yIncrement = -0.01f;
	} 
	else if (yMove <= 0.0f) { 
		yIncrement = 0.01f;
	}

	//loop through and draw the cubes 
	for (int pos = 0; pos < 46; pos++)
	{
	m4Translate = glm::translate(IDENTITY_M4, vector3(xValues[pos] + xMove, yValues[pos] + yMove, 0.0f));

	//translate and then scale
	m4Model = m4Translate * m4Scale;

	//scale then rotate
	//m4Model = m4Scale * m4Translate;
	
	m_pMesh[pos]->Render(m4Projection, m4View, m4Model);
	}	

	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
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
	for (int i = 0; i < 46; i++)
	{
		SafeDelete(m_pMesh[i]);
	}

	//release GUI
	ShutdownGUI();
}