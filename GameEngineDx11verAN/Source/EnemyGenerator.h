#pragma once
#include <vector>
#include <DirectXMath.h>
using namespace DirectX;

class RandomNum;
class MapData_RL;
struct DungeonMap_Info;

// “G‚ğ¶¬‚·‚é‚½‚ß‚¾‚¯‚ÌƒNƒ‰ƒX

class EnemyGenerator
{
public:
	EnemyGenerator();
	~EnemyGenerator();
	void GenerateEnemies(DungeonMap_Info* const _dng, std::vector<std::vector<MapData_RL>>& _maprl, std::vector<XMFLOAT3>& _spawn);

private:
	RandomNum* rdn_;
};