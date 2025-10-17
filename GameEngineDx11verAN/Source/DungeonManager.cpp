#include "DungeonManager.h"
#include "DungeonGenerator.h"
#include "../Engine/Model.h"
#include <algorithm> 
#include <vector>

namespace
{
	const size_t AREACOUNT_MIN = 12; // マップの区分け最小数
	const size_t AREACOUNT_RAND = 4; // マップの区分け数加算
	const size_t ROOMLENGTH_MIN_X = 4; // 部屋のX座標の最小サイズ
	const size_t ROOMLENGTH_MIN_Y = 4; // 部屋のY座標の最小サイズ
	const size_t ROOMLENGTH_RAND_X = 1; // 部屋のX座標のサイズ加算
	const size_t ROOMLENGTH_RAND_Y = 1; // 部屋のY座標のサイズ加算
	const size_t MAPX_RLk = 32; //マップ縦サイズ
	const size_t MAPY_RLk = 32;   //マップ横サイズ
}

DungeonManager::DungeonManager(GameObject* _parent)
	: dungeonGenerator_(nullptr), GameObject(_parent, "DungeonManager")
{
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
	maprl = std::vector<std::vector<MapData_RL>>(MAPX_RLk, std::vector<MapData_RL>(MAPY_RLk, MAPCHIP_WALL));
}

DungeonManager::~DungeonManager()
{
}

void DungeonManager::Initialize()
{
	wallModel_ = Model::Load("Box.fbx");

	dungeonGenerator_ = new DungeonGenerator();
	dungeonGenerator_->Initialize();
	dungeonGenerator_->GenerateDungeon(dungeonMapInfo_, maprl);
}

void DungeonManager::Update()
{
}

void DungeonManager::Draw()
{
	for (size_t i = 0; i < MAPX_RLk; ++i)
	{
		for (size_t j = 0;j < MAPY_RLk; ++j)
		{
			Transform mapTransform_;
			mapTransform_.position_ = { static_cast<float>(i) * 30.0f, 0.0f, static_cast<float>(j) * 30.0f };
			switch (maprl[i][j].mapData)
			{
			case MAPCHIP_WALL:
				mapTransform_.scale_ = { 10.0f, 10.0f, 10.0f };
				Model::SetTransform(wallModel_, mapTransform_);
				Model::Draw(wallModel_);
			}
		}
	}
}

void DungeonManager::Release()
{
}
