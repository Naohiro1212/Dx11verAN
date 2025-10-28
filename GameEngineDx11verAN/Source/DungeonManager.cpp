#include "DungeonManager.h"
#include "DungeonGenerator.h"
#include "../Engine/Model.h"
#include "../Engine/Input.h"
#include "../Engine/Global.h"
#include <algorithm> 
#include <vector>
#include "Player.h"
#include "MiniMap.h"
#include "EnemyBox.h"

namespace
{
	const size_t AREACOUNT_MIN = 6; // マップの区分け最小数
	const size_t AREACOUNT_RAND = 2; // マップの区分け数加算
	const size_t ROOMLENGTH_MIN_X = 4; // 部屋のX座標の最小サイズ
	const size_t ROOMLENGTH_MIN_Y = 4; // 部屋のY座標の最小サイズ
	const size_t ROOMLENGTH_RAND_X = 2; // 部屋のX座標のサイズ加算
	const size_t ROOMLENGTH_RAND_Y = 2; // 部屋のY座標のサイズ加算
	const size_t MAPX_RLk = 32; //マップ縦サイズ
	const size_t MAPY_RLk = 32;   //マップ横サイズ
	const XMFLOAT3 MAPCHIP_SCALE = { 23.5f, 15.0f, 15.0f }; // 描画の際のスケール
	const float MAPTILE_SIZE = 30.0f;
}

DungeonManager::DungeonManager(GameObject* _parent)
	: dungeonGenerator_(nullptr), enemyGenerator_(nullptr),GameObject(_parent, "DungeonManager")
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
	objectName_ = "DungeonManager";
}

DungeonManager::~DungeonManager()
{
	for(auto* enemy : enemies_)
	{
		if (enemy)
		{
			enemy->KillMe();
		}
	}
	enemies_.clear();
	
	SAFE_DELETE(enemyGenerator_);
	SAFE_DELETE(dungeonGenerator_);
	SAFE_DELETE(dungeonMapInfo_);
}

void DungeonManager::Initialize()
{
	wallModel_ = Model::Load("Box.fbx");
	player_ = Instantiate<Player>(this);

	dungeonGenerator_ = new DungeonGenerator();
	enemyGenerator_ = new EnemyGenerator();
	dungeonGenerator_->Initialize();
	DungeonReset();
	mapTransform_.scale_ = MAPCHIP_SCALE;

	// ミニマップを最後に生成
	// だいぶ時間がかかってしまいそうだったので後回し
//	Instantiate<MiniMap>(this);
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
			mapTransform_.position_ = { static_cast<float>(i) * MAPTILE_SIZE, 0.0f, static_cast<float>(j) * MAPTILE_SIZE };
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
	// 既存の敵を削除
	for (auto* enemy : enemies_)
	{
		if (enemy)
		{
			enemy->KillMe();
		}
	}
	enemies_.clear();

	// ダンジョン再生成
	// 一度床で塗りつぶす
	maprl = std::vector<std::vector<MapData_RL>>(MAPX_RLk, std::vector<MapData_RL>(MAPY_RLk, MAPCHIP_WALL));
	dungeonGenerator_->GenerateDungeon(dungeonMapInfo_, maprl);

	// 最初の部屋にプレイヤー開始位置を指定
	playerStartPos_.x = static_cast<float>((dungeonMapInfo_->mapRoom[0][2] + dungeonMapInfo_->mapRoom[0][0]) / 2) * MAPTILE_SIZE;
	playerStartPos_.y = 0.0f;
	playerStartPos_.z = static_cast<float>((dungeonMapInfo_->mapRoom[0][3] + dungeonMapInfo_->mapRoom[0][1]) / 2) * MAPTILE_SIZE;
	player_->SetPosition(playerStartPos_);

	// 敵の位置取得・敵生成
	enemyPositions_.clear();
	enemyGenerator_->GenerateEnemies(dungeonMapInfo_, maprl, enemyPositions_);

	for (size_t i = 0; i < enemyPositions_.size(); ++i)
	{
		EnemyBox* enemy_ = Instantiate<EnemyBox>(this);
		enemy_->SetPosition(enemyPositions_[i]);
		enemies_.push_back(enemy_);
	}
}

XMFLOAT3 DungeonManager::GetPlayerPosition() const
{
	return player_->GetPosition();
}
