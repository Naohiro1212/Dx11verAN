#pragma once
#include "../Engine/GameObject.h"
#include <vector>

struct DungeonMap_Info;
class RandomNum;

// XY軸を示すための定義
enum :int
{
	RL_COUNT_X,
	RL_COUNT_Y
};

class DungeonGenerator :
	public GameObject
{
public:
	DungeonGenerator(GameObject* _parent);
	~DungeonGenerator();
	void Initialize() override;
	void Draw() override;
	void Release() override;

	template<typename T>
	int GenerateDungeon(DungeonMap_Info* const _dng, T& _maprl);

private:
	// 自動生成されたダンジョンのマップデータ
	std::vector<std::vector<int>> dungeonMap_;
	// ダンジョンマップの情報
	DungeonMap_Info dungeonMapInfo;
	size_t mapData_;
	RandomNum rdn_;
};