#include "DungeonManager.h"
#include "DungeonGenerator.h"
#include "../Engine/Model.h"
#include <vector>
#include "Wall.h"

namespace
{
	size_t AREACOUNT_MIN = 3; // マップの区分け最小数
	size_t AREACOUNT_RAND = 4; // マップの区分け数加算
	size_t ROOMLENGTH_MIN_X = 5; // 部屋のX座標の最小サイズ
	size_t ROOMLENGTH_MIN_Y = 5; // 部屋のY座標の最小サイズ
	size_t ROOMLENGTH_RAND_X = 2; // 部屋のX座標のサイズ加算
	size_t ROOMLENGTH_RAND_Y = 2; // 部屋のY座標のサイズ加算
	const size_t MAPX_RLk = 64; //マップ縦サイズ
	const size_t MAPY_RLk = 32;   //マップ横サイズ

}

DungeonManager::DungeonManager(GameObject* _parent)
	: dungeonGenerator_(nullptr)
{
	// DungeonMap_RLのことを指している
	dungeonMapInfo_ = new DungeonMap_Info
	{
		AREACOUNT_MIN,
		AREACOUNT_RAND,
		ROOMLENGTH_MIN_X,
		ROOMLENGTH_MIN_Y,
		ROOMLENGTH_RAND_X,
		ROOMLENGTH_RAND_Y
	};

	// マップのデータを表す整数値を持つベクター
	maprl = std::vector<std::vector<MapData_RL>>(MAPX_RLk, std::vector<MapData_RL>(MAPY_RLk, 0));
}

DungeonManager::~DungeonManager()
{
}

void DungeonManager::Initialize()
{
	dungeonGenerator_ = new DungeonGenerator();
	dungeonGenerator_->Initialize();
	dungeonGenerator_->GenerateDungeon(dungeonMapInfo_, maprl);
	for (size_t i = 0; i < MAPX_RLk; ++i)
	{
		for (size_t j = 0; j < MAPY_RLk; ++j)
		{
			// 1なら壁、0なら床
			if (maprl[i][j].mapData == 1)
			{
				Instantiate<Wall>(this, XMFLOAT3(i * 30.0f, 0.0f, j * 30.0f));
			}
		}
	}
}

void DungeonManager::Update()
{
}

void DungeonManager::Draw()
{
}

void DungeonManager::Release()
{
}
