#include <cstdint>
#include <vector>
#include "DungeonGenerator.h"
#include "../Engine/Model.h"
#include "DungeonMap.h"
#include "../Engine/RandomNum.h"

namespace
{
	size_t AREACOUNT_MIN = 3; // マップの区分け最小数
	size_t AREACOUNT_RAND = 4; // マップの区分け数加算
	size_t ROOMLENGTH_MIN_X = 5; // 部屋のX座標の最小サイズ
	size_t ROOMLENGTH_MIN_Y = 5; // 部屋のY座標の最小サイズ
	size_t ROOMLENGTH_RAND_X = 2; // 部屋のX座標のサイズ加算
	size_t ROOMLENGTH_RAND_Y = 2; // 部屋のY座標のサイズ加算
}

DungeonGenerator::DungeonGenerator(GameObject* parent)
	: GameObject(parent, "TestScene")
{
	// 構造体の初期化
	dungeonMapInfo =
	{
		AREACOUNT_MIN,
		AREACOUNT_RAND,
		ROOMLENGTH_MIN_X,
		ROOMLENGTH_MIN_Y,
		ROOMLENGTH_RAND_X,
		ROOMLENGTH_RAND_Y
	};
	mapData_ = 1;
}

DungeonGenerator::~DungeonGenerator()
{
}

void DungeonGenerator::Initialize()
{
}

void DungeonGenerator::Draw()
{
}

void DungeonGenerator::Release()
{
}

template<typename T>
int DungeonGenerator::GenerateDungeon(DungeonMap_Info* const _dng, T& _maprl)
{
	if (_maprl.size() == 0 || _maprl.front().size() == 0) return -1;
	for (size_t i = 0; i < _maprl.size(); ++i)
	{
		for (size_t j = 0; j < _maprl[i].size(); ++j)
		{
			_maprl[i][j].mapData = 1;
		}
	}

	// マップの区分け数（部屋の個数）0~nまでの部屋ID
	_dng->mapDivCount = _dng->areaCountMin_ + (size_t)rdn_.GetRand((int)_dng->areaCountRand_);
	if (_dng->mapDivCount > 7)
	{
		return -1;
	}

	_dng->mapDiv[0][0] = (_maprl.size() - 1); // マップの区分け初期サイズX終点（マップの大きさX軸）
	_dng->mapDiv[0][1] = (_maprl.front().size() - 1); // マップの区分け初期サイズY終点（マップの大きさY軸）
	_dng->mapDiv[0][2] = 1; // マップの区分け初期サイズX始点（マップの大きさX軸）
	_dng->mapDiv[0][3] = 1; // マップの区分け初期サイズY始点（マップの大きさY軸）

	_dng->mapLoad[0][0] = 255; // 初期化用値
	_dng->mapLoad[0][1] = 255; // 初期化用値

	// マップを区分けしていく処理（区域を分割する処理）
	size_t divAfter_;
	// 区切りを入れる軸（X軸かY軸か）を示す
	int count_;

	for (size_t i = 1; i < _dng->mapDivCount; ++i)
	{
		// いままで作った区分けをランダムに指定（指定した区域を更に分割する）
		if (_dng->mapDiv[divAfter_][0] - _dng->mapDiv[divAfter_][2] > _dng->mapDiv[divAfter_][1] - _dng->mapDiv[divAfter_][3])
		{
			count_ = RL_COUNT_X;
		}
		else
		{
			count_ = RL_COUNT_Y;
		}

		if (dng->mapDiv[divAfter][count] - dng->mapDiv[divAfter][count + 2] < dng->divCountRand * 2 + 8)
		{
			size_t k = 0;
			for (size_t j = 1; j < _dng->mapDivCount; ++j)
			{
				if (_dng->mapDiv[j][0] - _dng->mapDiv[j][2] > k)
				{
					k = _dng->mapDiv[j][0] - _dng->mapDiv[j][2];
					divAfter_ = j;
					count_ = RL_COUNT_X;
				}
				if(_dng->mapDiv[j][1] - _dng->mapDiv[j][3] > k)
				{
					k = _dng->mapDiv[j][1] - _dng->mapDiv[j][3];
					divAfter_ = j;
					count_ = RL_COUNT_Y;
				}
			}
		}

		_dng->mapLoad[i][0] = divAfter_; // i番目の通路の前の部屋ID
		_dng->mapLoad[i][1] = count_;    // i番目の通路の方向

		for (size_t j = 1; j < i; ++j)
		{
			if (_dng->mapLoad[j][0] == divAfter_)
			{
				_dng->mapLoad[j][0] = i;
			}

			// X軸Y軸の設定
			_dng->mapDiv[i][count_] = _dng->mapDiv[divAfter_][count_ + 2] +
				(_dng->mapDiv[divAfter_][count_] - _dng->mapDiv[divAfter_][count_ + 2]) / 3 +
				(size_t)rdn_.GetRand((int)((dng->mapDiv[divAfter_][count_] - dng->mapDiv[divAfter_][count_ + 2]) / 3)); // 0.軸の右端の座標
			_dng->mapDiv[i][count_ + 2] = _dng->mapDiv[divAfter_][count_ + 2]; // 0.軸の左端の座標
			_dng->mapDiv[divAfter_][count_ + 2] = _dng->mapDiv[i][count_]; // divAfter_軸の左端の座標
		
			// count_とは逆の軸(count_がXならY,count_がYならX)
			_dng->mapDiv[i][abs(count_ - 1)] = _dng->mapDiv[divAfter_][abs(count_ - 1)]; // 軸の右端の座標
			_dng->mapDiv[i][abs(count_ - 1) + 2] = _dng->mapDiv[divAfter_][abs(count_ - 1) + 2]; // 軸の左端の座標
		}

		// 部屋を生成する処理
		for (size_t i = 0; i < _dng->mapDivCount; ++i) // 区分け
		{
			_dng->mapRoomPlayer[i] = 0; // プレイヤー初期化
			_dng->mapRoom[i][2] = _dng->mapDiv[i][2]; // 区分けX始点をマップX始点へと代入
			_dng->mapRoom[i][3] = _dng->mapDiv[i][3]; // 区分けY始点をマップY始点へと代入

			// X座標の部屋の長さを指定
			_dng->mapRoomPlayer[i][0] = _dng->mapDiv[i][2] + _dng->areaCountRand_ + (size_t)rdn_.GetRand((int)_dng->roomLengthRandX_); // プレイヤーがいる部屋のX座標の右端を決定している
			if (_dng->mapDiv[i][0] - _dng->mapDiv[i][2] < _dng->mapRoom[i][0] - _dng->mapRoom[i][2] + 5) // 部屋の長さが区分け範囲の長さと比較し、明らかに大きくなっていないかを判定する
			{
				_dng->mapRoom[i][0] = _dng->mapDiv[i][0] - 4; // 部屋のサイズが区分けサイズに収まるように面積を制限している
				if (_dng->mapDiv[i][0] - _dng->mapDiv[i][2] < _dng->mapRoom[i][0] - _dng->mapRoom[i][2] + 5) // 再度部屋の長さと区分け範囲の長さと比較
				{
					_dng->mapRoom[i][0] = _dng->mapDiv[i][2] + 1; // 部屋の右端を区切り範囲の左端に1足した位置に強制設定
				}
			}

			// Y座標の部屋の長さを指定
			_dng->mapRoomPlayer[i][1] = _dng->mapDiv[i][3] + _dng->roomLengthMinY_ + (size_t)rdn_.GetRand((int)_dng->roomLengthRandY_); // 部屋のY方向の終端座標を決める
			if (_dng->mapRoom[i][0] - _dng->mapDiv[i][2] <= 1 || _dng->mapRoom[i][1] - _dng->mapDiv[i][3] <= 1) // 部屋の幅もしくは高さが非常に狭い場合の判定
			{
				// 条件を満たす場合、部屋の端を区画の開始点に近い位置に修正
				_dng->mapRoom[i][0] = _dng->mapDiv[i][2] + 1;
				_dng->mapRoom[i][1] = _dng->mapDiv[i][3] + 1;
			}
			// 部屋の右端と上端を区割りの範囲内でランダムに拡張する幅を決める
			size_t expandWidth = (size_t)rdn_.GetRand((int)(_dng->mapDiv[i][0] - _dng->mapRoom[i][0] - 5)) + 2;
			size_t expandHeight = (size_t)rdn_.GetRand((int)(_dng->mapDiv[i][1] - _dng->mapRoom[i][1] - 5)) + 2;
			_dng->mapRoom[i][0] += expandWidth;
			_dng->mapRoom[i][2] += expandWidth;
			_dng->mapRoom[i][1] += expandHeight;
			_dng->mapRoom[i][3] += expandHeight;

			for (size_t j = _dng->mapRoom[i][2]; j < _dng->mapRoom[i][0]; ++j)
			{
				for (size_t k = _dng->mapRoom[i][3]; k < _dng->mapRoom[i][1]; ++k)
				{
					_maprl[j][k].mapData = 0;
				}
			}
		}

		// 通路を生成する処理
	}
}
