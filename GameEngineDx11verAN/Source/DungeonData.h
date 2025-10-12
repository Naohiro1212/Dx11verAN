#pragma once

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
	size_t mapDiv[8][4]{}; // マップの区域情報[部屋ID][X終点,Y終点,X始点,Y始点]
	size_t mapRoom[8][4]{}; // マップの部屋情報[部屋ID][X終点,Y終点,X始点,Y始点]
	size_t mapRoad[8][4]{}; // マップの通路情報[部屋ID(前)][つながる先の部屋ID(後), (0.X座標, 1.Y座標), (前)側の通路の位置 , (後)側の通路の位置]
	size_t mapRoomPlayer[8]{}; // マップのプレイヤーの開始位置[部屋ID]
};

// RogueLikeMapの事を指す
class MapData_RL
{
public:
	MapData_RL(const size_t var_) :mapData(var_) {}
	MapData_RL() = default;
	size_t mapData = 1; //マップ
};