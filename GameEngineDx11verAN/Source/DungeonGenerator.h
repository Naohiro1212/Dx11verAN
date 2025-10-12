#pragma once
#include <vector>

class RandomNum;
class MapData_RL;
struct DungeonMap_Info;

// XY軸を示すための定義
enum :int
{
	RL_COUNT_X,
	RL_COUNT_Y
};

class DungeonGenerator
{
public:
	DungeonGenerator();
	~DungeonGenerator();

	void Initialize();

	int GenerateDungeon(DungeonMap_Info* const _dng, std::vector<std::vector<MapData_RL>>& _maprl);

private:
	// 自動生成されたダンジョンのマップデータ
	std::vector<std::vector<int>> dungeonMap_;
	// ダンジョンマップの情報
	size_t mapData_;
	RandomNum* rdn_;
};