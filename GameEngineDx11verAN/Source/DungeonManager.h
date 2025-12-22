#pragma once
#include "../Engine/GameObject.h"
#include "EnemyGenerator.h"
#include "DungeonData.h"
#include <vector>

class Player;
class testEnemy;
class DungeonGenerator;
class EnemyGenerator;
class BoxCollider;

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
	void DungeonReset();

	// ミニマップ用のゲッター
	const std::vector<std::vector<MapData_RL>>& GetMap() const { return maprl; }
	XMFLOAT3 GetPlayerPosition() const;
	const std::vector<testEnemy*>& GetEnemies() const { return enemies_; }

	// 壁当たり判定用のゲッター
	const std::vector<BoxCollider*>& GetWallColliders() const { return wallColliders_; }

private:
	DungeonGenerator* dungeonGenerator_;
	DungeonMap_Info* dungeonMapInfo_;
	EnemyGenerator* enemyGenerator_;

	std::vector<std::vector<MapData_RL>> maprl;

	// 敵の位置の配列
	std::vector<XMFLOAT3> enemyPositions_;
	std::vector<testEnemy*> enemies_;

	// 壁のモデル
	int wallModel_;

	// マップ描画用の座標情報
	Transform mapTransform_;

	// プレイヤーの開始位置
	Player* player_;
	XMFLOAT3 playerStartPos_;

	// 壁のコライダー
	std::vector<BoxCollider*> wallColliders_;

	// 一定距離以内の壁コライダーキャッシュ
	std::vector<BoxCollider*> nearWallColliders_;

	bool needDungeonReset_;
};