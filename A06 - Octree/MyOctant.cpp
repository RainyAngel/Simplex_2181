#include "MyOctant.h"
using namespace Simplex;

Simplex::MyOctant::MyOctant() 
{ 
	Init(); 

	//set it to the very first octant, the big octant
	m_pRoot = this;

	//get the entity list and the count
	MyEntity** l_Entity_List = m_pEntityMngr->GetEntityList();
	uint iEntityCount = m_pEntityMngr->GetEntityCount();

	//create a list of all the min and max points
	std::vector<vector3> v3MaxMin_list; 
	for (uint i = 0; i < iEntityCount; ++i)
	{
		//push the entities onto the big list so the big octant is aware there are entities in it
		uint entity_id = m_pEntityMngr->GetEntityIndex(l_Entity_List[i]->GetUniqueID());
		m_EntityList.push_back(entity_id);
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

	//makes sure the rigid body is tight to the big octant
	m_pOctantBody->MakeCubic();

	Subdivide();

	//call on root and it will work itself down to the actual leaves
	ConstructList();

	AssignIDtoEntity();
}

Simplex::MyOctant::MyOctant(vector3 a_v3Center, float a_fSize)
{
	Init();
	std::vector<vector3> v3MaxMin_List;
	v3MaxMin_List.push_back(a_v3Center - vector3(a_fSize));
	v3MaxMin_List.push_back(a_v3Center + vector3(a_fSize));
	m_pOctantBody = new MyRigidBody(v3MaxMin_List);
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

void Simplex::MyOctant::IsColliding(uint a_uRBIndex)
{
	MyRigidBody* pRB = m_pEntityMngr->GetEntity(a_uRBIndex)->GetRigidBody();
	if (pRB->IsColliding(m_pOctantBody))
	{
		m_EntityList.push_back(a_uRBIndex);
	}
}

void Simplex::MyOctant::Display(vector3 a_v3Color)
{
	m_pOctantBody->AddToRenderList();

	for (uint i = 0; i < 8; i++)
	{
		if (m_pChild[i])
			m_pChild[i]->Display(a_v3Color); 
	}
}

void Simplex::MyOctant::ClearEntityList(void)
{
	m_EntityList.clear();
}

void Simplex::MyOctant::Subdivide(void)
{
	//will stop the recursive process
	if (m_uLevel > 1)
		return;

	//allocate the smaller octants of this big octant
	vector3 v3Center = m_pOctantBody->GetCenterLocal();
	vector3 v3HalfWidth = m_pOctantBody->GetHalfWidth();
	float fSize = (v3HalfWidth.x) / 2.0f;
	float fCenters = fSize;

	m_pChild[0] = new MyOctant(v3Center + vector3(fCenters, fCenters, fCenters), fSize);
	m_pChild[1] = new MyOctant(v3Center + vector3(-fCenters, fCenters, fCenters), fSize);
	m_pChild[2] = new MyOctant(v3Center + vector3(-fCenters, -fCenters, fCenters), fSize);
	m_pChild[3] = new MyOctant(v3Center + vector3(fCenters, -fCenters, fCenters), fSize);

	m_pChild[4] = new MyOctant(v3Center + vector3(fCenters, fCenters, -fCenters), fSize);
	m_pChild[5] = new MyOctant(v3Center + vector3(-fCenters, fCenters, -fCenters), fSize);
	m_pChild[6] = new MyOctant(v3Center + vector3(-fCenters, -fCenters, -fCenters), fSize);
	m_pChild[7] = new MyOctant(v3Center + vector3(fCenters, -fCenters, -fCenters), fSize);

	for (uint i = 0; i < 8; i++)
	{
		//check every entity under the child
		for (uint j = 0; j < m_EntityList.size(); j++)
		{
			m_pChild[i]->IsColliding(m_EntityList[j]);
		}
		
		//increment level
		m_pChild[i]->m_uLevel = m_uLevel + 1;
		//set the child as the new "big octant"
		m_pChild[i]->m_pParent = this;
		//set all children's root to the big octant
		m_pChild[i]->m_pRoot = m_pRoot;
		//recursive
		m_pChild[i]->Subdivide();
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
	//if first child in the array is a nullptr, then the octant has no children
	if (m_pChild[0] == nullptr) { 
		return true;
	}
	else {
		return false;
	}
} 

bool Simplex::MyOctant::ContainsMoreThan(uint a_nEntities)
{
	if (m_EntityList.size() > a_nEntities) {
		return true;
	}
	else {
		return false;
	}
}

void Simplex::MyOctant::KillBranches(void)
{
	for (uint i = 0; i < 8; i++) 
	{
		SafeDelete(m_pChild[i]); 
	}
}

void Simplex::MyOctant::ConstructTree(uint a_nMaxLevel)
{
}

void Simplex::MyOctant::AssignIDtoEntity(void)
{
	//to get the leaves
	for (uint i = 0; i < m_lChild.size(); i++)
	{
		m_lChild[i]->m_uID = i;
		//to get the entities in the leaves
		for (uint j = 0; j < m_lChild[i]->m_EntityList.size(); j++)
		{
			//to assign entities their leaves
			m_pEntityMngr->AddDimension(m_lChild[i]->m_EntityList[j], m_lChild[i]->m_uID);
		}
	}
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
	}
}

void Simplex::MyOctant::Init(void)
{
	//get the singletons of the mesh manager and enitity manager
	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();
}

void Simplex::MyOctant::ConstructList(void)
{
	//if it is a leaf, save it
	if (IsLeaf())
	{
		m_pRoot->m_lChild.push_back(this);
	}
	//if not, recurse this until you find a leaf
	else {
		for (uint i = 0; i < 8; i++)
		{
			m_pChild[i]->ConstructList();
		}
	}
}
