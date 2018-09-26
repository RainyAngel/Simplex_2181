#include "MyMesh.h"
void MyMesh::Init(void)
{
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = ShaderManager::GetInstance();
}
void MyMesh::Release(void)
{
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}
MyMesh::MyMesh()
{
	Init();
}
MyMesh::~MyMesh() { Release(); }
MyMesh::MyMesh(MyMesh& other)
{
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}
MyMesh& MyMesh::operator=(MyMesh& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyMesh temp(other);
		Swap(temp);
	}
	return *this;
}
void MyMesh::Swap(MyMesh& other)
{
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}
void MyMesh::CompleteMesh(vector3 a_v3Color)
{
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i)
	{
		m_lVertexCol.push_back(a_v3Color);
	}
}
void MyMesh::AddVertexPosition(vector3 a_v3Input)
{
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}
void MyMesh::AddVertexColor(vector3 a_v3Input)
{
	m_lVertexCol.push_back(a_v3Input);
}
void MyMesh::CompileOpenGL3X(void)
{
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++)
	{
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}
void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{
	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader); 

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));
	
	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);  

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.f, -1.f);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);
	glDisable(GL_POLYGON_OFFSET_LINE);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
	//A--B
	//This will make the triangle A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);

	AddVertexPosition(a_vTopLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopRight);
}
void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue,-fValue, fValue); //0
	vector3 point1( fValue,-fValue, fValue); //1
	vector3 point2( fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue,-fValue,-fValue); //4
	vector3 point5( fValue,-fValue,-fValue); //5
	vector3 point6( fValue, fValue,-fValue); //6
	vector3 point7(-fValue, fValue,-fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	//MAKE CIRCULAR BASE
	//for saving the previous points
	float prevX = 0.0f;
	float prevZ = 0.0f;

	//for using previously saved points for connecting circle to tip of cone
	std::vector<float> baseX;
	std::vector<float> baseZ;

	//for getting the points
	float x = 0.0f;
	float z = 0.0f;

	//angle for finding the points
	float theta = 0.0f;

	//cycle through the number of subdivisions
	for (int i = 1; i <= a_nSubdivisions; i++)
	{
		//find/reset theta
		theta = (360.0f / a_nSubdivisions) * (PI / 180.0f);

		//if at the beginning, start at radius, 0, 0
		if (i == 1) {
			theta = theta * i;
			x = a_fRadius * cosf(theta);
			z = a_fRadius * sinf(theta);

			baseX.push_back(a_fRadius);
			baseZ.push_back(0);

			AddTri(vector3(0, 0, 0), vector3(a_fRadius, 0, 0), vector3(x, 0, z));
		}
		//else save previous points and connect all points with triangle
		else {
			theta = theta * i;
			prevX = x;
			prevZ = z;

			baseX.push_back(x);
			baseZ.push_back(z);

			x = a_fRadius * cosf(theta);
			z = a_fRadius * sinf(theta);

			AddTri(vector3(0, 0, 0), vector3(prevX, 0, prevZ), vector3(x, 0, z));
		}
	}

	//CONNECT CIRCULAR BASE TO POINT 
	vector3 tipPoint(0, a_fHeight, 0);

	for (int i = 0; i <= a_nSubdivisions - 1; i++)
	{
		if (i == a_nSubdivisions - 1) {
			AddTri(vector3(baseX[0], 0, baseZ[0]), vector3(baseX[i], 0, baseZ[i]), tipPoint);
		}
		else {
			AddTri(vector3(baseX[i + 1], 0, baseZ[i + 1]), vector3(baseX[i], 0, baseZ[i]), tipPoint);
		}
	}

		// Adding information about color
		CompleteMesh(a_v3Color);
		CompileOpenGL3X();
}
void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	//MAKE CIRCULAR BASE AND TOP

	float prevX = 0.0f;
	float prevZ = 0.0f;

	//for using previously saved points for connecting circle to top and bottom circle
	std::vector<float> baseX;
	std::vector<float> baseZ;

	float x = 0.0f;
	float z = 0.0f;

	float theta = 0.0f;

	for (int i = 1; i <= a_nSubdivisions; i++)
	{
		theta = (360.0f / a_nSubdivisions) * (PI / 180.0f);

		//if at the beginning, start at radius, 0, 0
		if (i == 1) {
			theta = theta * i;
			x = a_fRadius * cosf(theta);
			z = a_fRadius * sinf(theta);

			baseX.push_back(a_fRadius);
			baseZ.push_back(0);

			AddTri(vector3(0, 0, 0), vector3(a_fRadius, 0, 0), vector3(x, 0, z));
			AddTri(vector3(a_fRadius, a_fHeight, 0), vector3(0, a_fHeight, 0), vector3(x, a_fHeight, z));
		}
		//else save previous points and connect all points with quad
		else {
			theta = theta * i;
			prevX = x;
			prevZ = z;

			baseX.push_back(x);
			baseZ.push_back(z);

			x = a_fRadius * cosf(theta);
			z = a_fRadius * sinf(theta);

			AddTri(vector3(0, 0, 0), vector3(prevX, 0, prevZ), vector3(x, 0, z));
			AddTri(vector3(prevX, a_fHeight, prevZ), vector3(0, a_fHeight, 0), vector3(x, a_fHeight, z));
		}
	}

	//SIDE PANELS
	for (int i = 0; i <= a_nSubdivisions - 1; i++)
	{
		if (i == a_nSubdivisions - 1) {
			AddQuad(vector3(baseX[0], 0, baseZ[0]), vector3(baseX[i], 0, baseZ[i]), vector3(baseX[0], a_fHeight, baseZ[0]), vector3(baseX[i], a_fHeight, baseZ[i]));
		}
		else {
			AddQuad(vector3(baseX[i + 1], 0, baseZ[i + 1]), vector3(baseX[i], 0, baseZ[i]), vector3(baseX[i + 1], a_fHeight, baseZ[i + 1]), vector3(baseX[i], a_fHeight, baseZ[i]));
		}
	}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	//for saving the previous points
	float prevInnerX = 0.0f;
	float prevInnerZ = 0.0f;
	float prevOuterX = 0.0f;
	float prevOuterZ = 0.0f;

	//for using previously saved points for connecting circle to the outer circle and inner circle
	std::vector<float> baseInnerX;
	std::vector<float> baseInnerZ;
	std::vector<float> baseOuterX;
	std::vector<float> baseOuterZ;

	float innerX = 0.0f;
	float innerZ = 0.0f;
	float outerX = 0.0f;
	float outerZ = 0.0f;

	float theta = 0.0f;

	for (int i = 1; i <= a_nSubdivisions; i++)
	{
		theta = (360.0f / a_nSubdivisions) * (PI / 180.0f);

		if (i == 1) {
			theta = theta * i;
			innerX = a_fInnerRadius * cosf(theta);
			innerZ = a_fInnerRadius * sinf(theta);

			outerX = a_fOuterRadius * cosf(theta);
			outerZ = a_fOuterRadius * sinf(theta);

			baseInnerX.push_back(a_fInnerRadius);
			baseInnerZ.push_back(0);

			baseOuterX.push_back(a_fOuterRadius);
			baseOuterZ.push_back(0);

			//top
			AddQuad(vector3(a_fInnerRadius, a_fHeight, 0), vector3(innerX, a_fHeight, innerZ),
				vector3(a_fOuterRadius, a_fHeight, 0), vector3(outerX, a_fHeight, outerZ));
			//bottom
			AddQuad(vector3(innerX, 0, innerZ), vector3(a_fInnerRadius, 0, 0),
				vector3(outerX, 0, outerZ), vector3(a_fOuterRadius, 0, 0));
		}
		else {
			theta = theta * i;
			prevInnerX = innerX;
			prevInnerZ = innerZ;

			prevOuterX = outerX;
			prevOuterZ = outerZ;

			baseInnerX.push_back(innerX);
			baseInnerZ.push_back(innerZ);

			baseOuterX.push_back(outerX);
			baseOuterZ.push_back(outerZ);

			innerX = a_fInnerRadius * cosf(theta);
			innerZ = a_fInnerRadius * sinf(theta);

			outerX = a_fOuterRadius * cosf(theta);
			outerZ = a_fOuterRadius * sinf(theta);

			//top
			AddQuad(vector3(prevInnerX, a_fHeight, prevInnerZ), vector3(innerX, a_fHeight, innerZ),
				vector3(prevOuterX, a_fHeight, prevOuterZ), vector3(outerX, a_fHeight, outerZ));
			//bottom
			AddQuad(vector3(innerX, 0, innerZ), vector3(prevInnerX, 0, prevInnerZ),
				vector3(outerX, 0, outerZ), vector3(prevOuterX, 0, prevOuterZ));
		}
	}
	
	//ADD SIDE PANELS
	for (int i = 0; i <= a_nSubdivisions - 1; i++)
	{
		if (i == a_nSubdivisions - 1) {
			//inner
			AddQuad(vector3(baseInnerX[i], 0, baseInnerZ[i]), vector3(baseInnerX[0], 0, baseInnerZ[0]),
				vector3(baseInnerX[i], a_fHeight, baseInnerZ[i]), vector3(baseInnerX[0], a_fHeight, baseInnerZ[0]));
			//outer
			AddQuad(vector3(baseOuterX[0], 0, baseOuterZ[0]), vector3(baseOuterX[i], 0, baseOuterZ[i]),
				vector3(baseOuterX[0], a_fHeight, baseOuterZ[0]), vector3(baseOuterX[i], a_fHeight, baseOuterZ[i]));
		}
		else {
			//inner
			AddQuad(vector3(baseInnerX[i], 0, baseInnerZ[i]), vector3(baseInnerX[i + 1], 0, baseInnerZ[i + 1]),
				vector3(baseInnerX[i], a_fHeight, baseInnerZ[i]), vector3(baseInnerX[i + 1], a_fHeight, baseInnerZ[i + 1]));
			//outer
			AddQuad(vector3(baseOuterX[i + 1], 0, baseOuterZ[i + 1]), vector3(baseOuterX[i], 0, baseOuterZ[i]),
				vector3(baseOuterX[i + 1], a_fHeight, baseOuterZ[i + 1]), vector3(baseOuterX[i], a_fHeight, baseOuterZ[i]));
		}
	}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsA < 3)
		a_nSubdivisionsA = 3;
	if (a_nSubdivisionsA > 360)
		a_nSubdivisionsA = 360;

	if (a_nSubdivisionsB < 3)
		a_nSubdivisionsB = 3;
	if (a_nSubdivisionsB > 360)
		a_nSubdivisionsB = 360;

	Release();
	Init();

	// Replace this with your code
	//"make" circle of given subdivisions and outerRadius here (x and z) -- save the points
	//"make" circles of given subdivisions with the radius of outerRadius - innerRadius (x and y and z)
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 1)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}
	if (a_nSubdivisions > 36)
		a_nSubdivisions = 36;

	Release();
	Init();

	std::vector<float> baseX;
	std::vector<float> baseY;
	std::vector<float> baseZ;

	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	float theta = (360.0f / a_nSubdivisions) * (PI / 180.0f);

	//top and bottom of sphere
	vector3 top = vector3(0, a_fRadius, 0);
	vector3 bottom = vector3(0, -a_fRadius, 0);


	//for the linear equations for finding the equal distances between the subdivision lines
	float a = -(2 * a_fRadius) / (a_nSubdivisions + 1);

	//calculate the different heights for each
	for (int k = 1; k <= a_nSubdivisions; k++)
	{
		//calculate the height for each subdivisions
		y = (a * k) + a_fRadius;
		//push to the y vector
		baseY.push_back(y);
	}

	float rads = 0.0f;

	//h represents all circle layers
	for (int h = 0; h < a_nSubdivisions; h++)
	{
		//a_fRadius = sqrtf(a_fRadius);
		//gets the radius for each subdivision level (pythagroean theorem)
		rads = sqrtf((a_fRadius * a_fRadius) - (baseY[h] * baseY[h]));

		//save these as the first two points of every circle "layer"
		baseX.push_back(rads);
		baseZ.push_back(0);

		for (int j = 1; j < a_nSubdivisions; j++)
		{
			//make the other points on the circle based on the radius calculated above
			x = rads * cos(theta * j);
			z = rads * sin(theta * j);

			baseX.push_back(x);
			baseZ.push_back(z);

			//for getting the points needed to create and connect the circle
			float currPnt = j + (h * a_nSubdivisions);
			float prevPnt = j + ((h - 1) * a_nSubdivisions);

			//create the different layers of circles
			//when h is 0, the first layer is tris to a point
			if (h == 0) {
				AddTri(vector3(baseX[j - 1], baseY[h], baseZ[j-1]), top, vector3(baseX[j], baseY[h], baseZ[j]));
			}
			else {
				AddQuad(vector3(baseX[currPnt], baseY[h], baseZ[currPnt]), vector3(baseX[currPnt - 1], baseY[h], baseZ[currPnt - 1]), 
					vector3(baseX[prevPnt], baseY[h - 1], baseZ[prevPnt]), 
					vector3(baseX[prevPnt - 1], baseY[h - 1], baseZ[prevPnt - 1]));
			}
			//draw the bottom piece
			if (h == a_nSubdivisions - 1) {
				AddTri(vector3(baseX[j - 1], baseY[h], baseZ[j - 1]), vector3(baseX[j], baseY[h], baseZ[j]), bottom);
			}
		}
		//make the last triangle of the top
		if (h == 0) {
			AddTri(vector3(baseX[a_nSubdivisions - 1], baseY[h], baseZ[a_nSubdivisions - 1]), top, vector3(baseX[0], baseY[h], baseZ[0]));
		}
		//make the last quad
		else {
			AddQuad(vector3(baseX[a_nSubdivisions * h], baseY[h], baseZ[a_nSubdivisions * h]), 
				vector3(baseX[(a_nSubdivisions * (h + 1)) - 1], baseY[h], baseZ[(a_nSubdivisions * (h + 1)) - 1]),
				vector3(baseX[(a_nSubdivisions * (h - 1))], baseY[h - 1], baseZ[(a_nSubdivisions * (h - 1))]),
				vector3(baseX[(a_nSubdivisions * h) - 1], baseY[h - 1], baseZ[(a_nSubdivisions * h) - 1]));
		}
		//make the last triangle of the bottom
		if (h == a_nSubdivisions - 1) {
			AddTri(vector3(baseX[a_nSubdivisions - 1], baseY[h], baseZ[a_nSubdivisions - 1]), vector3(baseX[0], baseY[h], baseZ[0]), bottom);
		}
	}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}