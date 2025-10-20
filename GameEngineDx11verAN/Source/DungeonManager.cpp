#include "DungeonManager.h"
#include "DungeonGenerator.h"
#include "../Engine/Model.h"
#include "../Engine/Input.h"
#include <algorithm> 
#include <vector>
#include "Player.h"

namespace
{
	const size_t AREACOUNT_MIN = 8; // マップの区分け最小数
	const size_t AREACOUNT_RAND = 2; // マップの区分け数加算
	const size_t ROOMLENGTH_MIN_X = 5; // 部屋のX座標の最小サイズ
	const size_t ROOMLENGTH_MIN_Y = 5; // 部屋のY座標の最小サイズ
	const size_t ROOMLENGTH_RAND_X = 2; // 部屋のX座標のサイズ加算
	const size_t ROOMLENGTH_RAND_Y = 2; // 部屋のY座標のサイズ加算
	const size_t MAPX_RLk = 32; //マップ縦サイズ
	const size_t MAPY_RLk = 32;   //マップ横サイズ
	const XMFLOAT3 MAPCHIP_SCALE = { 23.5f, 15.0f, 15.0f }; // 描画の際のスケール
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

}

DungeonManager::~DungeonManager()
{
}

void DungeonManager::Initialize()
{
	wallModel_ = Model::Load("Box.fbx");

	dungeonGenerator_ = new DungeonGenerator();
	dungeonGenerator_->Initialize();
	DungeonReset();
	mapTransform_.scale_ = MAPCHIP_SCALE;

	playerStartPos_ = dungeonGenerator_->GetPlayerStartPos();
	player_ = Instantiate<Player>(this);
	player_->SetPosition(playerStartPos_);
}

void DungeonManager::Update()
{
	// pキーでダンジョン再生成テスト
	if (Input::IsKeyDown(DIK_P))
	{
		DungeonReset();
	}
}

void DungeonManager::Draw()
{
	for (size_t i = 0; i < MAPX_RLk; ++i)
	{
		for (size_t j = 0;j < MAPY_RLk; ++j)
		{
			mapTransform_.position_ = { static_cast<float>(i) * 30.0f, 0.0f, static_cast<float>(j) * 30.0f };
			switch (maprl[i][j].mapData)
			{
			case MAPCHIP_WALL:
				Model::SetTransform(wallModel_, mapTransform_);
				Model::Draw(wallModel_);
			}
		}
	}
}

void DungeonManager::Release()
{
}

void DungeonManager::DungeonReset()
{
	// ダンジョン再生成
	// 一度床で塗りつぶす
	maprl = std::vector<std::vector<MapData_RL>>(MAPX_RLk, std::vector<MapData_RL>(MAPY_RLk, MAPCHIP_WALL));
	dungeonGenerator_->GenerateDungeon(dungeonMapInfo_, maprl);
}
