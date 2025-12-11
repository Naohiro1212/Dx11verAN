#pragma once
#include <vector>

enum MapChipType : size_t
{
	MAPCHIP_WALL = 1, // 壁や未使用
	MAPCHIP_FLOOR = 2, // 床
	MAPCHIP_ROAD = 3, // 通路
	MAPCHIP_PLAYERSTART = 4, // プレイヤーの開始位置
};

struct DungeonMap_Info
{
	// 生成される部屋の数（生成される区域の数）
	size_t areaCountMin_; // マップの区分け最小数
	size_t areaCountRand_; // マップの区分け数加算

	// 生成される部屋のサイズ
	size_t roomLengthMinX_; // 部屋のX座標の最小サイズ
	size_t roomLengthMinY_; // 部屋のY座標の最小サイズ
	size_t roomLengthRandX_; // 部屋のX座標のサイズ加算
	size_t roomLengthRandY_; // 部屋のY座標のサイズ加算

	size_t mapDivCount{}; // マップの区分け数（部屋の個数）0~nまでの部屋ID
	std::vector<std::vector<size_t>> mapDiv; // マップの区域情報[部屋ID][X終点,Y終点,X始点,Y始点]
	std::vector<std::vector<size_t>> mapRoom; // マップの部屋情報[部屋ID][X終点,Y終点,X始点,Y始点]
	std::vector<std::vector<size_t>> mapRoad; // マップの通路情報[部屋ID(前)][つながる先の部屋ID(後), (0.X座標, 1.Y座標), (前)側の通路の位置 , (後)側の通路の位置]
	std::vector<size_t> mapRoomPlayer; // マップのプレイヤーの開始位置[部屋ID]
};

// RogueLikeMapの事を指す
class MapData_RL
{
public:
	MapData_RL(const size_t var_ = MAPCHIP_WALL) :mapData(var_) {}
	MapData_RL() = default;
	size_t mapData = MAPCHIP_WALL;

private:
};