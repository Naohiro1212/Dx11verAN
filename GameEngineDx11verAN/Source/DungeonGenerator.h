#pragma once
#include <vector>
#include <DirectXMath.h>
using namespace DirectX;

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
	XMFLOAT3 GetPlayerStartPos() const { return playerStartPos_; }

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
	// 乱数生成クラス
	RandomNum* rdn_;
	// 安全に乱数を取得する関数
	size_t SafeRand(size_t _max);
	// プレイヤーの開始位置
	XMFLOAT3 playerStartPos_;
	// 区域が分割成功したかどうかのフラグ
	bool divided_;
	// 区域再分割用の変数
	size_t retry_ = 0;
};