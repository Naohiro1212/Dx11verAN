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

	// 符号付きで差を取り、負なら0を返す
	static int DiffClamped(size_t a, size_t b, int extra = 0)
	{
		long da = static_cast<long>(a);
		long db = static_cast<long>(b);
		long d = da - db - extra;
		return d > 0 ? static_cast<int>(d) : 0;
	}

	// 自動生成されたダンジョンのマップデータ
	std::vector<std::vector<int>> dungeonMap_;
	// ダンジョンマップの情報
	size_t mapData_;
	RandomNum* rdn_;
	int SafeRand(int _max);
};