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
#include "Portal.h"

#include "../Engine/Debug.h"

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
	const XMFLOAT3 MAPCHIP_SCALE = { 0.3f, 0.40f, 0.3f }; // 描画の際のスケール
	const float MAPTILE_SIZE = 30.0f;
	const XMFLOAT3 COLLIDER_SIZE = { 40.0f, 30.0f, 33.0f };
	const float WALL_DRAW_DISTANCE = 500.0f * 500.0f; // 壁の描画距離（距離の2乗で管理）
	const int MAX_FLOOR = 3;
	const float PLAYER_START_HEIGHT = 10.0f; // プレイヤーの開始高さ
	const float PORTAL_HEIGHT = 10.0f;
	const float CENTER_OFFSET = 0.5f;
	const float PLAYER_TO_PORTAL_DISTANCE = 30.0f; // プレイヤーとポータルの距離がこの値以下で再生成可能
}

DungeonManager::DungeonManager(GameObject* _parent)
	: dungeonGenerator_(nullptr), enemyGenerator_(nullptr), GameObject(_parent, "DungeonManager"), wallModel_(-1), player_(nullptr), portal_(nullptr),
	resetDungeon_(false), nearPortal_(false)
{
	dungeonMapInfo_ = new DungeonMap_Info
	{
		AREACOUNT_MIN,
		AREACOUNT_RAND,
		ROOMLENGTH_MIN_X,
		ROOMLENGTH_MIN_Y,
		ROOMLENGTH_RAND_X,
		ROOMLENGTH_RAND_Y,
	};

	rdn_ = new RandomNum();
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

	// 3) プレイヤー/ポータル生成（Resetで参照するため先に)
	player_ = Instantiate<Player>(GetParent());
	portal_ = Instantiate<Portal>(GetParent());

	// 階層初期化
	nowFloor_ = 0;

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

	// 死んだ敵を配列から削除
	enemies_.erase(
		std::remove_if(enemies_.begin(), enemies_.end(),
			[](testEnemy* enemy) {
				return enemy == nullptr || enemy->IsDead();
			}),
		enemies_.end()
	);

	if(resetDungeon_)
	{
		DungeonReset();
		resetDungeon_ = false;
	}

	// 敵の数が0で、ポータルとプレイヤーが近づきキーを押すとダンジョン再生成
	StageClearCheck();
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
				XMFLOAT3 wallPos = { static_cast<float>(i) * MAPTILE_SIZE, 0.5f, static_cast<float>(j) * MAPTILE_SIZE };

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
	// 階数を進める
	nowFloor_++;

	// 階数が一定以上を超えたら早期リターン
	if (nowFloor_ > MAX_FLOOR)
	{
		return;
	}

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
			enemy->SetWallColliders({});
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
	playerStartPos_.y = PLAYER_START_HEIGHT;
	playerStartPos_.z = static_cast<float>((dungeonMapInfo_->mapRoom[0][3] + dungeonMapInfo_->mapRoom[0][1]) / 2) * MAPTILE_SIZE;

	// プレイヤーの位置適用
	if (player_)
	{
		player_->SetPosition(playerStartPos_);
	}

	// ポータル位置設定（最後の部屋の中央）
	portalPos_.x = static_cast<float>((dungeonMapInfo_->mapRoom[dungeonMapInfo_->mapDivCount - 1][2]
		+ dungeonMapInfo_->mapRoom[dungeonMapInfo_->mapDivCount - 1][0]) / 2) * MAPTILE_SIZE;
	portalPos_.y = PORTAL_HEIGHT;
	portalPos_.z = static_cast<float>((dungeonMapInfo_->mapRoom[dungeonMapInfo_->mapDivCount - 1][3]
		+ dungeonMapInfo_->mapRoom[dungeonMapInfo_->mapDivCount - 1][1]) / 2) * MAPTILE_SIZE;

	// ポータルの位置適用
	if (portal_)
	{
		portal_->SetPosition(portalPos_);
	}

	// 敵の位置取得・敵生成
	enemyPositions_.clear();
	int enemyCount = rdn_->GetRand(1, nowFloor_); // 階数に応じて敵の数をランダムに決定
	dungeonMapInfo_->enemyCount = enemyCount; // 階数に応じて敵の数を増やす
	enemyGenerator_->GenerateEnemies(dungeonMapInfo_, maprl, enemyPositions_);

	for (size_t i = 0; i < enemyPositions_.size(); ++i)
	{
		testEnemy* enemy_ = Instantiate<testEnemy>(GetParent());
		enemy_->SetPosition(enemyPositions_[i]);
		enemies_.push_back(enemy_);
	}

	// 壁のコライダー生成
	wallColliders_.clear();
	for (size_t j = 0;j < MAPX_RLk; ++j)
	{
		// 壁開始を探す
		size_t i = 0;
		while (i < MAPX_RLk)
		{
			// 壁開始を探す
			while (i < MAPX_RLk && maprl[i][j].mapData != MAPCHIP_WALL) i++;
			if (i >= MAPX_RLk) break;

			// 連続区間の終端まで進む
			size_t start = i;
			while (i < MAPX_RLk && maprl[i][j].mapData == MAPCHIP_WALL) i++;
			size_t end = i;

			// コライダー生成
			const float width = (end - start) * MAPTILE_SIZE;
			const float centerX = ((start + end - 1) * CENTER_OFFSET) * MAPTILE_SIZE;
			const float centerZ = j * MAPTILE_SIZE;

			BoxCollider* wallCollider_ = new BoxCollider(
				{ centerX, /*高さセンタ*/ (COLLIDER_SIZE.y * CENTER_OFFSET), centerZ },
				{ width, COLLIDER_SIZE.y, COLLIDER_SIZE.z }  // 横幅をまとめる
			);

			AddCollider(wallCollider_);
			wallCollider_->SetRole(Collider::Role::Static);
			wallColliders_.push_back(wallCollider_);
		}
	}

	player_->SetWallColliders(wallColliders_);

	for (auto* enemy : enemies_)
	{
		if (enemy)
		{
			enemy->SetWallColliders(wallColliders_);
		}
	}
}

void DungeonManager::StageClearCheck()
{
	if (enemies_.empty())
	{
		// ポータル起動！ trueだと回転する
		portal_->SetActive(true);

		// プレイヤーとポータルの距離計算
		XMFLOAT3 playerPos = player_->GetPosition();
		XMFLOAT3 portalPos = portal_->GetPosition();
		float dx = playerPos.x - portalPos.x;
		float dz = playerPos.z - portalPos.z;
		float distSq = dx * dx + dz * dz;
		if (distSq <= PLAYER_TO_PORTAL_DISTANCE * PLAYER_TO_PORTAL_DISTANCE)
		{
			nearPortal_ = true;
			// Eキーで再生成
			if (Input::IsKeyDown(DIK_E)) // Eキーで再生成
			{
				resetDungeon_ = true;
			}
		}
		else
		{
			nearPortal_ = false;
		}
	}
	else
	{
		// 敵が存在するのであれば、ポータルは非アクティブ
		portal_->SetActive(false);
	}
}

XMFLOAT3 DungeonManager::GetPlayerPosition() const
{
	return player_->GetPosition();
}

int DungeonManager::GetMaxFloor() const
{
	return MAX_FLOOR;
}
