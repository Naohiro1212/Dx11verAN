#pragma once
#include "Engine/GameObject.h"
#include <vector>

struct DungeonMap_Info
{
	// 生成される部屋の数（生成される区域の数）
	size_t areaCountMin_; // マップの区分け最小数
	size_t roomCountRand_;

};

class DungeonGenerator :
	public GameObject
{
public:
	DungeonGenerator(GameObject* _parent);
	~DungeonGenerator();
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release() override;
	void GenerateDungeon(int _dWidth, int _dHeight);

private:
	// 仮の壁、床用の箱モデル
	int hBoxModel_;
	// 自動生成されたダンジョンのマップデータ
	std::vector<std::vector<int>> dungeonMap_;
	int dungeonWidth_;
	int dungeonHeight_;
};