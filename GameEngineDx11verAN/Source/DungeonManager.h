#pragma once
#include "../Engine/GameObject.h"
#include "DungeonData.h"
#include <vector>

class Wall;
class DungeonGenerator;

class DungeonManager 
	: public GameObject
{
public:
	DungeonManager(GameObject* _parent);
	~DungeonManager();
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release() override;

private:
	DungeonGenerator* dungeonGenerator_;
	DungeonMap_Info* dungeonMapInfo_;
	std::vector<std::vector<MapData_RL>> maprl;
	int wallModel_;

	// WallÉvÅ[Éã
	std::vector<Wall*> wallPool_;
	size_t maxWall_;
};