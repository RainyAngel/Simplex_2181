#include "Octree.h"
using namespace Simplex;

Simplex::MyOctant::MyOctant() { Init(); }

Simplex::MyOctant::MyOctant(vector3 a_v3Center, float a_fSize)
{
	//save the values
	m_v3Center = a_v3Center;
	m_fSize = a_fSize;

	//get min and max
	m_v3Max = m_v3Center + vector3(a_fSize);
	m_v3Min = m_v3Center - vector3(a_fSize);
}

Simplex::MyOctant::MyOctant(MyOctant const & other)
{
	//copy everything from other
	//m_uOctantCount = other.m_uOctantCount;
	//m_uMaxLevel = other.m_uMaxLevel;
	//m_uIdealEntityCount = other.m_uIdealEntityCount;

	m_uID = other.m_uID;
	m_uLevel = other.m_uLevel;
	m_uChildren = other.m_uChildren;

	m_fSize = other.m_fSize;

	m_pMeshMngr = other.m_pMeshMngr;
	m_pEntityMngr = other.m_pEntityMngr;

	m_v3Center = other.m_v3Center;
	m_v3Min = other.m_v3Min;
	m_v3Max = other.m_v3Max;
	
	for (uint i = 0; i < 8; i++)
	{
		if(other.m_pChild[i] != NULL)
			m_pChild[i] = new MyOctant(*other.m_pChild[i]);
	}

	m_EntityList = other.m_EntityList;

	//this copy constructor should only be called when at the root
	m_pRoot = this;

	//creates the tree
	if(m_uLevel == 0)
		ConstructList();
}

MyOctant & Simplex::MyOctant::operator=(MyOctant const & other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyOctant temp(other);
		Swap(temp);
	}
	return *this;
}

Simplex::MyOctant::~MyOctant(void)
{
	Release();
}

void Simplex::MyOctant::Swap(MyOctant & other)
{
	//std::swap(m_uOctantCount, other.m_uOctantCount);
	//std::swap(m_uMaxLevel, other.m_uMaxLevel);
	//std::swap(m_uIdealEntityCount, other.m_uIdealEntityCount);

	std::swap(m_uID, other.m_uID);
	std::swap(m_uLevel, other.m_uLevel);
	std::swap(m_uChildren, other.m_uChildren);

	std::swap(m_fSize, other.m_fSize);

	std::swap(m_pMeshMngr, other.m_pMeshMngr);
	std::swap(m_pEntityMngr, other.m_pEntityMngr);

	std::swap(m_v3Center, other.m_v3Center);
	std::swap(m_v3Min, other.m_v3Min);
	std::swap(m_v3Max, other.m_v3Max);

	if (other.m_pParent != NULL) {
		std::swap(m_pParent, other.m_pParent);
	}

	for (uint i = 0; i < 8; i++)
	{
		if (other.m_pChild[i] != NULL)
			std::swap(m_pChild[i], other.m_pChild[i]);
	}

	std::swap(m_EntityList, other.m_EntityList);

	std::swap(m_pRoot, other.m_pRoot);

	std::swap(m_lChild, other.m_lChild);
}

float Simplex::MyOctant::GetSize(void)
{
	return m_fSize;
}

vector3 Simplex::MyOctant::GetCenterGlobal(void)
{
	return m_v3Center;
}

vector3 Simplex::MyOctant::GetMinGlobal(void)
{
	return m_v3Min;
}

vector3 Simplex::MyOctant::GetMaxGlobal(void)
{
	return m_v3Max;
}

bool Simplex::MyOctant::IsColliding(uint a_uRBIndex)
{
	std::vector<MyEntity*> l_Entity_List = m_pEntityMngr->GetEntityList();
	uint iEntityCount = l_Entity_List.size();
	for (uint i = 0; i < iEntityCount; ++i)
	{
		MyRigidBody* pRB = l_Entity_List[i]->GetRigidBody();
		if (pRB->IsColliding(m_pOctantBody))
		{
			l_Entity_List[i]->AddDimension(a_uRBIndex);
		}
	}
}

void Simplex::MyOctant::Display(uint a_nIndex, vector3 a_v3Color)
{
}

void Simplex::MyOctant::Display(vector3 a_v3Color)
{
}

void Simplex::MyOctant::DisplayLeafs(vector3 a_v3Color)
{
}

void Simplex::MyOctant::ClearEntityList(void)
{
	m_EntityList.clear();
}

void Simplex::MyOctant::Subdivide(void)
{
	//allocate the smaller octants of this big octant
	for (uint i = 0; i < 8; i++)
	{
		m_pChild[i] = new MyOctant();
	}
}

MyOctant * Simplex::MyOctant::GetChild(uint a_nChild)
{
	return m_pChild[a_nChild];
}

MyOctant * Simplex::MyOctant::GetParent(void)
{
	return m_pParent;
}

bool Simplex::MyOctant::IsLeaf(void)
{
	return false;
}

bool Simplex::MyOctant::ContainsMoreThan(uint a_nEntities)
{
	return false;
}

void Simplex::MyOctant::KillBranches(void)
{
}

void Simplex::MyOctant::ConstructTree(uint a_nMaxLevel)
{
}

void Simplex::MyOctant::AssignIDtoEntity(void)
{
}

uint Simplex::MyOctant::GetOctantCount(void)
{
	return uint();
}

void Simplex::MyOctant::Release(void)
{
	for (uint i = 0; i < 8; i++)
	{
		SafeDelete(m_pChild[i]);
		m_pChild[i] = NULL;
	}
}

void Simplex::MyOctant::Init(void)
{
	//get the singletons of the mesh manager and enitity manager
	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	//get the entity list and the count
	std::vector<MyEntity*> l_Entity_List = m_pEntityMngr->GetEntityList();
	uint iEntityCount = l_Entity_List.size();

	//create a list of all the min and max points
	std::vector<vector3> v3MaxMin_list;
	for (uint i = 0; i < iEntityCount; ++i)
	{
		MyRigidBody* pRG = l_Entity_List[i]->GetRigidBody();
		vector3 v3Min = pRG->GetMinGlobal();
		vector3 v3Max = pRG->GetMaxGlobal();
		v3MaxMin_list.push_back(v3Min);
		v3MaxMin_list.push_back(v3Max);
	}

	//initialize the children of the current octant to nullptr 
	for (uint i = 0; i < 8; i++)
	{
		m_pChild[i] = nullptr;
	}

	//create a "rigid body" for all the points where it will find the actual min, max, and center for you
	m_pOctantBody = new MyRigidBody(v3MaxMin_list);

	IsColliding(m_uID);
}

void Simplex::MyOctant::ConstructList(void)
{
}
