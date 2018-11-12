#include "Octree.h"
using namespace Simplex;

Simplex::MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntityCount)
{
	//save the values
	//m_uMaxLevel = a_nMaxLevel;
	//m_uIdealEntityCount = a_nIdealEntityCount;

	//find the size of the first octant (the big one)
	uint count = m_pEntityMngr->GetEntityCount();

	m_v3Min = m_pEntityMngr->GetRigidBody(0)->GetMinGlobal();
	m_v3Max = m_pEntityMngr->GetRigidBody(0)->GetMaxGlobal();

	for (uint i = 1; i < count; i++)
	{
		//get the min and max dependent on all the rigid bodies in the world 
		if (m_v3Min.x > m_pEntityMngr->GetRigidBody(i)->GetMinGlobal().x)
			m_v3Min.x = m_pEntityMngr->GetRigidBody(i)->GetMinGlobal().x;
		if (m_v3Min.y > m_pEntityMngr->GetRigidBody(i)->GetMinGlobal().y)
			m_v3Min.y = m_pEntityMngr->GetRigidBody(i)->GetMinGlobal().y;
		if (m_v3Min.z > m_pEntityMngr->GetRigidBody(i)->GetMinGlobal().z)
			m_v3Min.z = m_pEntityMngr->GetRigidBody(i)->GetMinGlobal().z;

		if (m_v3Max.x < m_pEntityMngr->GetRigidBody(i)->GetMaxGlobal().x)
			m_v3Max.x = m_pEntityMngr->GetRigidBody(i)->GetMaxGlobal().x;
		if (m_v3Max.y < m_pEntityMngr->GetRigidBody(i)->GetMaxGlobal().y)
			m_v3Max.y = m_pEntityMngr->GetRigidBody(i)->GetMaxGlobal().y;
		if (m_v3Max.z < m_pEntityMngr->GetRigidBody(i)->GetMaxGlobal().z)
			m_v3Max.z = m_pEntityMngr->GetRigidBody(i)->GetMaxGlobal().z;
	}

	//get the center of first octant
	m_v3Center = (m_v3Max - m_v3Min) / 2;
}

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
	return false;
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
}

void Simplex::MyOctant::Subdivide(void)
{
}

MyOctant * Simplex::MyOctant::GetChild(uint a_nChild)
{
	return nullptr;
}

MyOctant * Simplex::MyOctant::GetParent(void)
{
	return nullptr;
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
	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();
}

void Simplex::MyOctant::ConstructList(void)
{
}
