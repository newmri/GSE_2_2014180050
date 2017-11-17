#include "stdafx.h"
#include "Define.h"
#include "Objects.h"

CSceneMgr* CSceneMgr::m_instance = nullptr;

void CSceneMgr::Init()
{
	m_id = 0;
	this->InitNorthTeam();
	this->InitSouthTeam();
	m_time = GetTickCount();
	for(int i = 0; i < TEAM_END - 1; ++i) m_characterTime[i] = GetTickCount();
	m_characterTime[NORTH] = 0;
	cout << "SceneManager is initialized!" << endl;
	
}

void CSceneMgr::InitNorthTeam()
{
	
	// Left Building
	m_objects[NORTH].emplace_back(
		FACTORYMANAGER->CreateObj(
			ObjectInfo(this->GetID(), OWNER::NONE, TEAMTYPE::NORTH, OBJTYPE::OBJECT_BUILDING,
				Pos(-BUILDING_LEFT_XPOS, NORTH_BUILDING_YPOS),
				BUILDING_SIZE, Color(1.0f, 0.0f, 0.0f, 1.0f))));

	// Mid Building
	m_objects[NORTH].emplace_back(
		FACTORYMANAGER->CreateObj(
			ObjectInfo(this->GetID(), OWNER::NONE, TEAMTYPE::NORTH, OBJTYPE::OBJECT_BUILDING,
				Pos(0.0f, NORTH_BUILDING_YPOS + 40.0f),
				BUILDING_SIZE, Color(1.0f, 0.0f, 0.0f, 1.0f))));

	// Right Building
	m_objects[NORTH].emplace_back(
		FACTORYMANAGER->CreateObj(
			ObjectInfo(this->GetID(), OWNER::NONE, TEAMTYPE::NORTH, OBJTYPE::OBJECT_BUILDING,
				Pos(BUILDING_LEFT_XPOS, NORTH_BUILDING_YPOS),
				BUILDING_SIZE, Color(1.0f, 0.0f, 0.0f, 1.0f))));
}
void CSceneMgr::InitSouthTeam()
{

	// Left Building
	m_objects[SOUTH].emplace_back(
		FACTORYMANAGER->CreateObj(
			ObjectInfo(this->GetID(), OWNER::NONE, TEAMTYPE::SOUTH, OBJTYPE::OBJECT_BUILDING,
				Pos(-BUILDING_LEFT_XPOS, -NORTH_BUILDING_YPOS),
				BUILDING_SIZE, Color(0.0f, 0.0f, 1.0f, 1.0f))));

	// Mid Building
	m_objects[SOUTH].emplace_back(
		FACTORYMANAGER->CreateObj(
			ObjectInfo(this->GetID(), OWNER::NONE, TEAMTYPE::SOUTH, OBJTYPE::OBJECT_BUILDING,
				Pos(0.0f, -(NORTH_BUILDING_YPOS + 40.0f)),
				BUILDING_SIZE, Color(0.0f, 0.0f, 1.0f, 1.0f))));

	// Right Building
	m_objects[SOUTH].emplace_back(
		FACTORYMANAGER->CreateObj(
			ObjectInfo(this->GetID(), OWNER::NONE, TEAMTYPE::SOUTH, OBJTYPE::OBJECT_BUILDING,
				Pos(BUILDING_LEFT_XPOS, -NORTH_BUILDING_YPOS),
				BUILDING_SIZE, Color(0.0f, 0.0f, 1.0f, 1.0f))));
}

void CSceneMgr::CheckCollision()
{
	for (int k = 0; k < TEAM_END; ++k) {
		for (auto& d : m_objects[k]) {
			for (int i = 0; i < m_objects[k].size(); ++i) {

				for (int j = 0; j < m_objects[(k + 1) % TEAM_END].size(); ++j) d->CheckCollision(m_objects[(k + 1) % TEAM_END][j]);
				

				if (d->GetObjType() == OBJECT_CHARACTER || d->GetObjType() == OBJECT_BUILDING) {
					for (auto& sd : m_shootObjects[(k + 1) % TEAM_END]) d->CheckCollision(sd);
					
				}
			}

		}
	}

}
void CSceneMgr::RemoveObject()
{
	for (int i = 0; i < TEAM_END; ++i) {
		{
			vector<shared_ptr<CObject>>::iterator itor = m_objects[i].begin();
			while (itor != m_objects[i].end()) {
				if ((*itor)->DoHavetoBeRemoved()) itor = m_objects[i].erase(itor);
				else ++itor;
			}
		}

		{
			vector<shared_ptr<CObject>>::iterator itor = m_shootObjects[i].begin();
			while (itor != m_shootObjects[i].end()) {
				if ((*itor)->DoHavetoBeRemoved()) itor = m_shootObjects[i].erase(itor);
				else ++itor;
			}
		}
	}
}

void CSceneMgr::CreateNorthCharacter()
{
	if (m_characterTime[NORTH] + NORTH_CHARACTER_TIME < GetTickCount()) {
		Pos pos(rand() % WINDOW_WIDTH / 4.0f, rand() % WINDOW_HEIGHT / 4.0f);

		if (rand() % 2 == 1) pos.x = -pos.x;

		ObjectInfo info(SCENEMANAGER->GetID(), OWNER::NONE,TEAMTYPE::NORTH, OBJTYPE::OBJECT_CHARACTER,
			pos, CHARACTER_SIZE, Color(1.0f, 0.0f, 0.0f, 1.0f));
		SCENEMANAGER->AddNorthObject(FACTORYMANAGER->CreateObj(info));
		m_characterTime[NORTH] = GetTickCount();
	}
}

void CSceneMgr::Render()
{
	const char* name;
	for (int i = 0; i < TEAM_END; ++i) {
		if (i == 0) name = "NorthBuilding";
		else name = "SouthBuilding";
		for (auto& d : m_objects[i]) {
			if (d->GetObjType() == OBJECT_BUILDING)
				m_renderer->DrawTexturedRect(d->GetPos().x, d->GetPos().y, d->GetPos().z,
					d->GetSize(), d->GetColor().r, d->GetColor().g, d->GetColor().b,
					d->GetColor().a, IMAGEMANAGER->GetImage()[name]);

			else
				m_renderer->DrawSolidRect(d->GetPos().x, d->GetPos().y, d->GetPos().z,
					d->GetSize(), d->GetColor().r, d->GetColor().g, d->GetColor().b,
					d->GetColor().a);
		}

		for (auto& d : m_shootObjects[i]) {
			m_renderer->DrawSolidRect(d->GetPos().x, d->GetPos().y, d->GetPos().z,
				d->GetSize(), d->GetColor().r, d->GetColor().g, d->GetColor().b,
				d->GetColor().a);
		}
	}
}

void CSceneMgr::Update(float time)
{

	this->CheckCollision();
	this->CreateNorthCharacter();

	for (int i = 0; i < TEAM_END; ++i) {
		for (auto& d : m_objects[i]) d->Update(time);
		for (auto& d : m_shootObjects[i]) d->Update(time);

	}

	this->RemoveObject();

}