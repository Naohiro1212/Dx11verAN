#pragma once
#include "../Engine/GameObject.h"
#include "DungeonData.h"
#include <vector>

class Player;
class DungeonGenerator;

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

private:
	DungeonGenerator* dungeonGenerator_;
	DungeonMap_Info* dungeonMapInfo_;
	std::vector<std::vector<MapData_RL>> maprl;
	// 壁のモデル
	int wallModel_;
	// マップ描画用の座標情報
	Transform mapTransform_;
	// プレイヤーの開始位置
	Player* player_;
	XMFLOAT3 playerStartPos_;
};