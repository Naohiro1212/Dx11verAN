#pragma once
#include "../Engine/GameObject.h"
#include "EnemyGenerator.h"
#include "DungeonData.h"
#include <vector>

class Player;
class EnemyBox;
class DungeonGenerator;
class EnemyGenerator;

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

private:
	DungeonGenerator* dungeonGenerator_;
	DungeonMap_Info* dungeonMapInfo_;
	EnemyGenerator* enemyGenerator_;

	std::vector<std::vector<MapData_RL>> maprl;
	// 敵の位置の配列
	std::vector<XMFLOAT3> enemyPositions_;
	std::vector<EnemyBox*> enemies_;
	// 壁のモデル
	int wallModel_;
	// マップ描画用の座標情報
	Transform mapTransform_;
	// プレイヤーの開始位置
	Player* player_;
	XMFLOAT3 playerStartPos_;
};