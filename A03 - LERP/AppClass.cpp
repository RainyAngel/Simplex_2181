#include "AppClass.h"
void Application::InitVariables(void)
{
	//Change this to your name and email
	m_sProgrammer = "Charley Link - crl4975@rit.edu";
	
	//Set the position and target of the camera
	//(I'm at [0,0,10], looking at [0,0,0] and up is the positive Y axis)
	m_pCameraMngr->SetPositionTargetAndUpward(AXIS_Z * 20.0f, ZERO_V3, AXIS_Y);

	//if the light position is zero move it
	if (m_pLightMngr->GetPosition(1) == ZERO_V3)
		m_pLightMngr->SetPosition(vector3(0.0f, 0.0f, 3.0f));

	//if the background is cornflowerblue change it to black (its easier to see)
	if (vector3(m_v4ClearColor) == C_BLUE_CORNFLOWER)
	{
		m_v4ClearColor = vector4(ZERO_V3, 1.0f);
	}
	
	//if there are no segments create 7
	if(m_uOrbits < 1)
		m_uOrbits = 7;

	float fSize = 1.0f; //initial size of orbits

	//creating a color using the spectrum 
	uint uColor = 650; //650 is Red
	//prevent division by 0
	float decrements = 250.0f / (m_uOrbits > 1? static_cast<float>(m_uOrbits - 1) : 1.0f); //decrement until you get to 400 (which is violet)

	/*
		This part will create the orbits, it start at 3 because that is the minimum subdivisions a torus can have
	*/
	uint uSides = 3; //start with the minimal 3 sides
	for (uint i = uSides; i < m_uOrbits + uSides; i++) // i == number of vertices in the shape
	{
		vector3 v3Color = WaveLengthToRGB(uColor); //calculate color based on wavelength
		m_shapeList.push_back(m_pMeshMngr->GenerateTorus(fSize, fSize - 0.1f, 3, i, v3Color)); //generate a custom torus and add it to the meshmanager
		outerRadius.push_back(fSize);
		innerRadius.push_back(fSize - 0.1f);
		fSize += 0.5f; //increment the size for the next orbit
		uColor -= static_cast<uint>(decrements); //decrease the wavelength
	}

	float rads = 0.0f;
	float theta = 0.0f;
	float x = 0.0f;
	float y = 0.0f;

	//for cycling through all the orbits
	for (uint j = 0; j < m_uOrbits; j++)
	{
		//first orbit
		for (uint k = 0; k < uSides + j; k++)
		{
			rads = (360.0f / (uSides + j)) * (PI / 180);
			theta = rads * k;

			x = outerRadius[j] * cos(theta);
			y = outerRadius[j] * sin(theta);

			std::cout << outerRadius[j] * cosf(theta) << std::endl;

			orbits.push_back(vector3(x, y, 0));
		}
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

	matrix4 m4View = m_pCameraMngr->GetViewMatrix(); //view Matrix
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix(); //Projection Matrix
	matrix4 m4Offset = IDENTITY_M4; //offset of the orbits, starts as the global coordinate system

	/*
		The following offset will orient the orbits as in the demo, start without it to make your life easier.
	*/
	//m4Offset = glm::rotate(IDENTITY_M4, 1.5708f, AXIS_Z);

	//for lerping
	static float percent = 0.0f;	//increments until the point is hit
	static int point = 0;	//keeps track of where the sphere should be going to
	static vector3 start = orbits[point];
	static vector3 end = orbits[point + 1];

	//calculate the current position
	static vector3 v3CurrentPos = ZERO_V3;

	if (point % (orbits.size()-1) == 0) {
		point = 0;
	}

	//set the start equal to the end
	start = orbits[point];
	//set end to the next point
	end = orbits[point + 1];

	//move the circle
	v3CurrentPos = glm::lerp(orbits[point], orbits[point + 1], percent);

	std::cout << "Start: " << start.x << " " << start.y << std::endl;
	std::cout << "End: " << end.x << " " << end.y << std::endl;

	//increment percent so the circle moves
	percent += 0.01f;

	//if the circle reaches the next point...
	if (percent >= 1.0f) {
		//move to the next point
		point++;
		//reset percent since it is starting at a new point
		percent = 0.0f;
	}

	/*if (point % (orbits.size() - 1) == 0) {
		start = orbits[point];
		end = orbits[0];
	}*/

	// draw a shapes
	for (uint i = 0; i < m_uOrbits; ++i)
	{
		//calculate the current position
		//vector3 v3CurrentPos = ZERO_V3; 

		m_pMeshMngr->AddMeshToRenderList(m_shapeList[i], glm::rotate(m4Offset, 1.5708f, AXIS_X));

		matrix4 m4Model = glm::translate(m4Offset, v3CurrentPos);

		//draw spheres
		m_pMeshMngr->AddSphereToRenderList(m4Model * glm::scale(vector3(0.1)), C_WHITE);
	}

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
	//release GUI
	ShutdownGUI();
}