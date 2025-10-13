#include <cstdint>
#include <vector>
#include "DungeonGenerator.h"
#include "../Engine/Model.h"
#include "../Engine/RandomNum.h"
#include "DungeonData.h"

DungeonGenerator::DungeonGenerator()
{
}

DungeonGenerator::~DungeonGenerator()
{
	if (rdn_ != nullptr) {
		delete rdn_;
		rdn_ = nullptr;
	}
}

void DungeonGenerator::Initialize()
{
	rdn_ = new RandomNum();
}

int DungeonGenerator::SafeRand(int _max)
{
	if (_max <= 0) return 0;
	return rdn_->GetRand(_max);
}

int DungeonGenerator::GenerateDungeon(DungeonMap_Info* const _dng, std::vector<std::vector<MapData_RL>>& _maprl)
{
	// GenerateDungeon の最初（_dng->mapDivCount を決める前）に追加
	for (size_t i = 0; i < 8; ++i) {
		for (size_t j = 0; j < 4; ++j) {
			_dng->mapDiv[i][j] = 0;
			_dng->mapRoom[i][j] = 0;
			_dng->mapRoad[i][j] = static_cast<size_t>(-1);
		}
		_dng->mapRoomPlayer[i] = 0;
	}

	if (_maprl.size() == 0 || _maprl.front().size() == 0) return -1;
	for (size_t i = 0; i < _maprl.size(); ++i)
	{
		for (size_t j = 0; j < _maprl[i].size(); ++j)
		{
			_maprl[i][j].mapData = 1;
		}
	}

	// マップの区分け数（部屋の個数）0~nまでの部屋ID
	_dng->mapDivCount = _dng->areaCountMin_ + (size_t)rdn_->GetRand((int)_dng->areaCountRand_);
	if (_dng->mapDivCount > 7)
	{
		return -1;
	}

	_dng->mapDiv[0][0] = (_maprl.size() - 1); // マップの区分け初期サイズX終点（マップの大きさX軸）
	_dng->mapDiv[0][1] = (_maprl.front().size() - 1); // マップの区分け初期サイズY終点（マップの大きさY軸）
	_dng->mapDiv[0][2] = 1; // マップの区分け初期サイズX始点（マップの大きさX軸）
	_dng->mapDiv[0][3] = 1; // マップの区分け初期サイズY始点（マップの大きさY軸）

	_dng->mapRoad[0][0] = static_cast<size_t>(-1); // 初期化用値
	_dng->mapRoad[0][1] = static_cast<size_t>(-1); // 初期化用値

	// マップを区分けしていく処理（区域を分割する処理）
	size_t divAfter_;
	// 区切りを入れる軸（X軸かY軸か）を示す
	int count_;

	for (size_t i = 1; i < _dng->mapDivCount; ++i)
	{
		//今まで作った区分けをランダムに指定(指定した区域をさらに区分けする)
		divAfter_ = (size_t)rdn_->GetRand((int)i - 1);

		// いままで作った区分けをランダムに指定（指定した区域を更に分割する）
		if (_dng->mapDiv[divAfter_][0] - _dng->mapDiv[divAfter_][2] > _dng->mapDiv[divAfter_][1] - _dng->mapDiv[divAfter_][3])
		{
			count_ = RL_COUNT_X;
		}
		else
		{
			count_ = RL_COUNT_Y;
		}

		if (_dng->mapDiv[divAfter_][count_] - _dng->mapDiv[divAfter_][count_ + 2] < _dng->areaCountRand_ * 2 + 8)
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
				if (_dng->mapDiv[j][1] - _dng->mapDiv[j][3] > k)
				{
					k = _dng->mapDiv[j][1] - _dng->mapDiv[j][3];
					divAfter_ = j;
					count_ = RL_COUNT_Y;
				}
			}
		}

		_dng->mapRoad[i][0] = divAfter_; // i番目の通路の前の部屋ID
		_dng->mapRoad[i][1] = count_;    // i番目の通路の方向

		for (size_t j = 1; j < i; ++j)
		{
			if (_dng->mapRoad[j][0] == divAfter_)
			{
				_dng->mapRoad[j][0] = i;
			}

			// X軸Y軸の設定
			// 符号付き差分を取り、負なら0にする
			int span = DiffClamped(_dng->mapDiv[divAfter_][count_], _dng->mapDiv[divAfter_][count_ + 2]);
			int third = span / 3;                    // 分割幅の下限
			int randPart = SafeRand(third);          // third が 0 以下なら 0 を返す
			_dng->mapDiv[i][count_] = _dng->mapDiv[divAfter_][count_ + 2] + static_cast<size_t>(third + randPart);
			_dng->mapDiv[i][count_ + 2] = _dng->mapDiv[divAfter_][count_ + 2]; // 0.軸の左端の座標
			_dng->mapDiv[divAfter_][count_ + 2] = _dng->mapDiv[i][count_]; // divAfter_軸の左端の座標

			// count_とは逆の軸(count_がXならY,count_がYならX)
			_dng->mapDiv[i][abs(count_ - 1)] = _dng->mapDiv[divAfter_][abs(count_ - 1)]; // 軸の右端の座標
			_dng->mapDiv[i][abs(count_ - 1) + 2] = _dng->mapDiv[divAfter_][abs(count_ - 1) + 2]; // 軸の左端の座標
		}
	}

	// 部屋を生成する処理
	for (size_t i = 0; i < _dng->mapDivCount; ++i) // 区分け
	{
		_dng->mapRoomPlayer[i] = 0; // プレイヤー初期化
		_dng->mapRoom[i][2] = _dng->mapDiv[i][2]; // 区分けX始点をマップX始点へと代入
		_dng->mapRoom[i][3] = _dng->mapDiv[i][3]; // 区分けY始点をマップY始点へと代入

		// X座標の部屋の長さを指定
		_dng->mapRoom[i][0] = _dng->mapDiv[i][2] + _dng->areaCountRand_ + (size_t)rdn_->GetRand((int)_dng->roomLengthRandX_); // プレイヤーがいる部屋のX座標の右端を決定している
		if (_dng->mapDiv[i][0] - _dng->mapDiv[i][2] < _dng->mapRoom[i][0] - _dng->mapRoom[i][2] + 5) // 部屋の長さが区分け範囲の長さと比較し、明らかに大きくなっていないかを判定する
		{
			_dng->mapRoom[i][0] = _dng->mapDiv[i][0] - 4; // 部屋のサイズが区分けサイズに収まるように面積を制限している
			if (_dng->mapDiv[i][0] - _dng->mapDiv[i][2] < _dng->mapRoom[i][0] - _dng->mapRoom[i][2] + 5) // 再度部屋の長さと区分け範囲の長さと比較
			{
				_dng->mapRoom[i][0] = _dng->mapDiv[i][2] + 1; // 部屋の右端を区切り範囲の左端に1足した位置に強制設定
			}
		}

		// Y座標の部屋の長さを指定
		_dng->mapRoom[i][1] = _dng->mapDiv[i][3] + _dng->roomLengthMinY_ + (size_t)rdn_->GetRand((int)_dng->roomLengthRandY_); // 部屋のY方向の終端座標を決める
		if (_dng->mapRoom[i][0] - _dng->mapDiv[i][2] <= 1 || _dng->mapRoom[i][1] - _dng->mapDiv[i][3] <= 1) // 部屋の幅もしくは高さが非常に狭い場合の判定
		{
			// 条件を満たす場合、部屋の端を区画の開始点に近い位置に修正
			_dng->mapRoom[i][0] = _dng->mapDiv[i][2] + 1;
			_dng->mapRoom[i][1] = _dng->mapDiv[i][3] + 1;
		}
		// 部屋の右端と上端を区割りの範囲内でランダムに拡張する幅を決める
		size_t expandWidth = static_cast<size_t>(SafeRand(DiffClamped(_dng->mapDiv[i][0], _dng->mapRoom[i][0], 5))) + 2;
		size_t expandHeight = static_cast<size_t>(SafeRand(DiffClamped(_dng->mapDiv[i][1], _dng->mapRoom[i][1], 5))) + 2;
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
	// 通路は2部屋間の細い道のことを指す
	// 道路を作るために2部屋をそれぞれ前と後で分ける
	// for文ですべての部屋をチェックして、前後の部屋をつなぐ通路を作る
	// まず、前の通路を作り、次に後の通路を作る
	// 最後に前と後の通路を繋げる
	size_t roomAfter_;
	for (size_t roomBefore_ = 0; roomBefore_ < _dng->mapDivCount; ++roomBefore_)
	{
		roomAfter_ = _dng->mapRoad[roomBefore_][0];

		// X座標の通路
		switch (_dng->mapRoad[roomBefore_][1])
		{
		case RL_COUNT_X:
			_dng->mapRoad[roomBefore_][2] = static_cast<size_t>(SafeRand(DiffClamped(_dng->mapRoom[roomBefore_][1], _dng->mapRoom[roomBefore_][3], 2)));
			_dng->mapRoad[roomBefore_][3] = static_cast<size_t>(SafeRand(DiffClamped(_dng->mapRoom[roomAfter_][1], _dng->mapRoom[roomAfter_][3], 2)));

			// 前の通路
			for (size_t j = _dng->mapRoom[roomBefore_][0]; j < _dng->mapDiv[roomBefore_][0]; ++j)
			{
				_maprl[j][_dng->mapRoad[roomBefore_][2] + _dng->mapRoom[roomAfter_][3]].mapData = 0; // 通路をマップチップに線画
			}

			// 後ろの通路
			for (size_t j = _dng->mapDiv[roomAfter_][2]; j < _dng->mapRoom[roomAfter_][2]; ++j)
			{
				_maprl[j][_dng->mapRoad[roomBefore_][3] + _dng->mapRoom[roomAfter_][3]].mapData = 0; // 通路をマップチップに線画
			}

			// 通路をつなぐ
			for (size_t j = _dng->mapRoad[roomBefore_][2] + _dng->mapRoom[roomBefore_][3]; j <= _dng->mapRoad[roomBefore_][3] + _dng->mapRoom[roomAfter_][3]; ++j)
			{
				_maprl[_dng->mapDiv[roomBefore_][0]][j].mapData = 0; // 通路をマップチップに線画 2~5（上から下）
			}
			for (size_t j = _dng->mapRoad[roomBefore_][3] + _dng->mapRoom[roomAfter_][3]; j <= _dng->mapRoad[roomBefore_][2] + _dng->mapRoom[roomBefore_][3]; ++j)
			{
				_maprl[_dng->mapDiv[roomBefore_][0]][j].mapData = 0; //通路をマップチップに線画 5から2(下から上)
			}
			break;

		case RL_COUNT_Y:
			_dng->mapRoad[roomBefore_][2] = static_cast<size_t>(SafeRand(DiffClamped(_dng->mapRoom[roomBefore_][0], _dng->mapRoom[roomBefore_][2], 2)));
			_dng->mapRoad[roomBefore_][3] = static_cast<size_t>(SafeRand(DiffClamped(_dng->mapRoom[roomAfter_][0], _dng->mapRoom[roomAfter_][2], 2)));

			// 前の通路
			for (size_t j = _dng->mapRoom[roomBefore_][1]; j < _dng->mapDiv[roomBefore_][1]; ++j)
			{
				_maprl[_dng->mapRoad[roomBefore_][2] + _dng->mapRoom[roomBefore_][2]][j].mapData = 0; // 通路をマップチップに線画
			}

			// 後の通路
			for (size_t j = _dng->mapDiv[roomAfter_][3]; j < _dng->mapRoom[roomAfter_][3]; ++j)
			{
				_maprl[_dng->mapRoad[roomBefore_][3] + _dng->mapRoom[roomAfter_][2]][j].mapData = 0;  // 通路をマップチップに線画
			}

			// 通路をつなぐ
			for (size_t j = _dng->mapRoad[roomBefore_][2] + _dng->mapRoom[roomBefore_][2]; j <= _dng->mapRoad[roomBefore_][3] + _dng->mapRoom[roomAfter_][2]; ++j)
			{
				_maprl[j][_dng->mapDiv[roomBefore_][1]].mapData = 0; //通路をマップチップに線画
			}
			for (size_t j = _dng->mapRoad[roomBefore_][3] + _dng->mapRoom[roomAfter_][2]; j <= _dng->mapRoad[roomBefore_][2] + _dng->mapRoom[roomBefore_][2]; ++j)
			{
				_maprl[j][_dng->mapDiv[roomBefore_][1]].mapData = 0; // 通路をマップチップに線画
			}
			break;
		}
	}
	return 0;
}