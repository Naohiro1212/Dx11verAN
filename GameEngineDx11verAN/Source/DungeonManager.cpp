#include "DungeonManager.h"
#include "DungeonGenerator.h"
#include "../Engine/Model.h"
#include "../Engine/Input.h"
#include "../Engine/Global.h"

#include <algorithm> 
#include <vector>
#include "Player.h"
#include "MiniMap.h"
#include "testEnemy.h"

namespace
{
	const size_t AREACOUNT_MIN = 4; // マップの区分け最小数
	const size_t AREACOUNT_RAND = 1; // マップの区分け数加算
	const size_t ROOMLENGTH_MIN_X = 3; // 部屋のX座標の最小サイズ
	const size_t ROOMLENGTH_MIN_Y = 3; // 部屋のY座標の最小サイズ
	const size_t ROOMLENGTH_RAND_X = 1; // 部屋のX座標のサイズ加算
	const size_t ROOMLENGTH_RAND_Y = 1; // 部屋のY座標のサイズ加算
	const size_t MAPX_RLk = 22; //マップ縦サイズ
	const size_t MAPY_RLk = 22;   //マップ横サイズ
	const XMFLOAT3 MAPCHIP_SCALE = { 0.3f, 0.3f, 0.3f }; // 描画の際のスケール
	const float MAPTILE_SIZE = 30.0f;
	const XMFLOAT3 COLLIDER_SIZE = { 40.0f, 30.0f, 33.0f };
	const float WALL_DRAW_DISTANCE = 500.0f * 500.0f; // 壁の描画距離（距離の2乗で管理）

}

DungeonManager::DungeonManager(GameObject* _parent)
	: dungeonGenerator_(nullptr), enemyGenerator_(nullptr), GameObject(_parent, "DungeonManager")
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
	for (auto* enemy : enemies_)
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
	// 1) モデルロード
	wallModel_ = Model::Load("wall.fbx");

	// 2) ジェネレーター生成・初期化
	dungeonGenerator_ = new DungeonGenerator();
	enemyGenerator_ = new EnemyGenerator();
	dungeonGenerator_->Initialize();

	// 3) プレイヤー生成（Resetで参照するため先に）
	player_ = Instantiate<Player>(GetParent());

	// 4) ダンジョン生成・壁/敵/コライダー設定・プレイヤー位置適用
	DungeonReset();

	// 5) マップ描画用スケール（必要ならここで）
	mapTransform_.scale_ = MAPCHIP_SCALE;
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
	// プレイヤーの現在位置を取得
	XMFLOAT3 playerPos_ = player_->GetPosition();

	for (size_t i = 0; i < MAPX_RLk; ++i)
	{
		for (size_t j = 0;j < MAPY_RLk; ++j)
		{
			if (maprl[i][j].mapData == MAPCHIP_WALL)
			{
				// 壁のワールド座標
				XMFLOAT3 wallPos = { static_cast<float>(i) * MAPTILE_SIZE, -6.0f, static_cast<float>(j) * MAPTILE_SIZE };

				// 距離計算
				float dx = wallPos.x - playerPos_.x;
				float dz = wallPos.z - playerPos_.z;
				float distSq = dx * dx + dz * dz;

				// 一定距離以内の壁のみ描画
				if (distSq <= WALL_DRAW_DISTANCE)
				{
					mapTransform_.position_ = wallPos;
					Model::SetTransform(wallModel_, mapTransform_);
					Model::Draw(wallModel_);
				}
			}
		}
	}
}

void DungeonManager::Release()
{
}

void DungeonManager::DungeonReset()
{
	// 既存のコライダーを削除
	for (auto* collider : wallColliders_)
	{
		if (collider)
		{
			RemoveCollider(collider);
		}
	}
	wallColliders_.clear();

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
	playerStartPos_.y = -5.0f;
	playerStartPos_.z = static_cast<float>((dungeonMapInfo_->mapRoom[0][3] + dungeonMapInfo_->mapRoom[0][1]) / 2) * MAPTILE_SIZE;

	// プレイヤーの位置適用
	if (player_)
	{
		player_->SetPosition(playerStartPos_);
	}

	// 敵の位置取得・敵生成
	enemyPositions_.clear();
	enemyGenerator_->GenerateEnemies(dungeonMapInfo_, maprl, enemyPositions_);

	for (size_t i = 0; i < enemyPositions_.size(); ++i)
	{
		testEnemy* enemy_ = Instantiate<testEnemy>(GetParent());
		enemy_->SetPosition(enemyPositions_[i]);
		enemies_.push_back(enemy_);
	}

	// 壁のコライダー生成
	for (size_t i = 0;i < MAPX_RLk; ++i)
	{
		for (size_t j = 0;j < MAPY_RLk; ++j)
		{
			if (maprl[i][j].mapData == MAPCHIP_WALL)
			{
				mapTransform_.position_ = {
					static_cast<float>(i) * MAPTILE_SIZE, 0.0f,
					static_cast<float>(j) * MAPTILE_SIZE };
				mapTransform_.scale_ = MAPCHIP_SCALE;
				BoxCollider* wallCollider_ = new BoxCollider(
					{ mapTransform_.position_.x,
					  mapTransform_.position_.y + (MAPCHIP_SCALE.y / 2.0f),
					  mapTransform_.position_.z },
					COLLIDER_SIZE);
				AddCollider(wallCollider_);
				wallCollider_->SetRole(Collider::Role::Static);
				wallColliders_.push_back(wallCollider_);
			}
		}
	}

	player_->SetWallColliders(wallColliders_);

	for (auto* enemy : enemies_)
	{
		enemy->SetWallColliders(wallColliders_);
	}
}

XMFLOAT3 DungeonManager::GetPlayerPosition() const
{
	return player_->GetPosition();
}