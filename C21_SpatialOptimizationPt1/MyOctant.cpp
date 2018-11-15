#include "MyOctant.h"
using namespace Simplex;
//  MyOctant
void MyOctant::Init(void)
{
	m_nData = 0;
	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	std::vector<MyEntity*> l_EntityList = m_pEntityMngr->GetEntityList();
	uint iEntityCount = l_EntityList.size();
	std::vector<vector3> v3MaxMin_List;

	for (uint i = 0; i < iEntityCount; i++)
	{
		MyRigidBody* pRG = l_EntityList[i]->GetRigidBody();
		vector3 v3Position = pRG->GetCenterGlobal();

		vector3 v3Min = pRG->GetMinGlobal();
		vector3 v3Max = pRG->GetMaxGlobal();

		v3MaxMin_List.push_back(v3Min);
		v3MaxMin_List.push_back(v3Max);
	}

	m_pRigidBody = new MyRigidBody(v3MaxMin_List);
	IsColliding();
}
void MyOctant::Swap(MyOctant& other)
{
	std::swap(m_nData, other.m_nData);
	std::swap(m_lData, other.m_lData);
}
void MyOctant::Release(void)
{
	m_lData.clear();
}
void Simplex::MyOctant::Display(void)
{
	m_pRigidBody->AddToRenderList();
	//m_pMeshMngr->AddWireCubeToRenderList(glm::scale(vector3(70)), C_BLUE);
}
void Simplex::MyOctant::IsColliding(void)
{
	std::vector<MyEntity*> l_EntityList = m_pEntityMngr->GetEntityList();
	uint iEntityCount = l_EntityList.size();
	std::vector<vector3> v3MaxMin_List;

	for (uint i = 0; i < iEntityCount; i++) {
		MyRigidBody* pRB = l_EntityList[i]->GetRigidBody();
		if (pRB->IsColliding(m_pRigidBody)) {
			l_EntityList[i]->AddDimension(m_iID);
		}
	}

	for (uint i = 0; i < 8; i++)
	{
		m_pChild[i] = nullptr;
	}
}

void MyOctant::Subdivide() {
	for (uint i = 0; i < 8; i++)
	{
		
	}
}
Simplex::MyOctant::MyOctant(vector3 m_v3Size)
{
}
//The big 3
MyOctant::MyOctant(){Init();}
MyOctant::MyOctant(MyOctant const& other)
{
	m_nData = other.m_nData;
	m_lData = other.m_lData;
}
MyOctant& MyOctant::operator=(MyOctant const& other)
{
	if(this != &other)
	{
		Release();
		Init();
		MyOctant temp(other);
		Swap(temp);
	}
	return *this;
}
MyOctant::~MyOctant(){Release();};
//Accessors
void MyOctant::SetData(int a_nData){ m_nData = a_nData; }
int MyOctant::GetData(void){ return m_nData; }
void MyOctant::SetDataOnVector(int a_nData){ m_lData.push_back(a_nData);}
int& MyOctant::GetDataOnVector(int a_nIndex)
{
	int nIndex = static_cast<int>(m_lData.size());
	assert(a_nIndex >= 0 && a_nIndex < nIndex);
	return m_lData[a_nIndex];
}
//--- Non Standard Singleton Methods

