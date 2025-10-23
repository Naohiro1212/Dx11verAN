#pragma once
#include <vector>
#include <DirectXMath.h>
using namespace DirectX;

class MapData_RL;
struct DungeonMap_Info;

class EnemyGenerator
{
public:
	EnemyGenerator();
	void GenerateEnemies(DungeonMap_Info* const _dng, std::vector<std::vector<MapData_RL>>& _maprl, std::vector<XMFLOAT3>& _spawn);

private:
};