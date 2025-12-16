#include <cstdint>
#include <vector>
#include <algorithm>
#include "DungeonGenerator.h"
#include "../Engine/Model.h"
#include "../Engine/RandomNum.h"
#include "DungeonData.h"

namespace
{
	// ダンジョン生成のリトライ上限回数
	const size_t MAXRETRY = 100;
	// 区域・部屋・通路の座標を表す配列の要素数
	const size_t AREA_COORD_COUNT = 4;
	// 区域分割の最小しきい値
	const size_t MIN_DIVIDABLE_MARGIN = 8;

}

DungeonGenerator::DungeonGenerator() : retry_(0), divided_(false), rdn_(nullptr)
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

size_t DungeonGenerator::SafeRand(size_t _max)
{
	return rdn_->GetRand(_max);
}

int DungeonGenerator::GenerateDungeon(DungeonMap_Info* const _dng, std::vector<std::vector<MapData_RL>>& _maprl)
{
	// マップの区分け数（部屋の個数）0~nまでの部屋ID
	_dng->mapDivCount = _dng->areaCountMin_ + (size_t)SafeRand((int)_dng->areaCountRand_);

	// マップを必要なサイズでリサイズ
	// 4つの値で区域を定義するため、内側の配列サイズは4で固定
	_dng->mapDiv.resize(_dng->mapDivCount, std::vector<size_t>(AREA_COORD_COUNT, 0));
	_dng->mapRoom.resize(_dng->mapDivCount, std::vector<size_t>(AREA_COORD_COUNT, 0));
	_dng->mapRoad.resize(_dng->mapDivCount, std::vector<size_t>(AREA_COORD_COUNT, static_cast<size_t>(-1)));
	_dng->mapRoomPlayer.resize(_dng->mapDivCount, 0);

	// GenerateDungeon の最初（_dng->mapDivCount を決める前）に追加
	for (size_t i = 0; i < _dng->mapDivCount; ++i) {
		for (size_t j = 0; j < AREA_COORD_COUNT; ++j) {
			_dng->mapDiv[i][j] = 0;
			_dng->mapRoom[i][j] = 0;
			_dng->mapRoad[i][j] = static_cast<size_t>(-1);
		}
		_dng->mapRoomPlayer[i] = 0;
	}

	// マップサイズの取得と初期化
	// mapDiv[num][0] : X終点
	// mapDiv[num][1] : Y終点
	// mapDiv[num][2] : X始点
	// mapDiv[num][3] : Y始点

	if (_maprl.size() == 0 || _maprl.front().size() == 0) return -1;

	_dng->mapDiv[0][0] = (_maprl.size() - 1); // マップの区分け初期サイズX終点（マップの大きさX軸）
	_dng->mapDiv[0][1] = (_maprl.front().size() - 1); // マップの区分け初期サイズY終点（マップの大きさY軸）
	_dng->mapDiv[0][2] = 1; // マップの区分け初期サイズX始点（マップの大きさX軸）
	_dng->mapDiv[0][3] = 1; // マップの区分け初期サイズY始点（マップの大きさY軸）

	_dng->mapRoad[0][0] = static_cast<size_t>(-1); // 初期化用値
	_dng->mapRoad[0][1] = static_cast<size_t>(-1); // 初期化用値

	// マップを区分けしていく処理（区域を分割する処理）
	size_t divAfter_;
	int count_;
	divided_ = false;

	retry_ = 0;
	for (size_t i = 1; i < _dng->mapDivCount && retry_ < MAXRETRY; ++i)
	{
		//今まで作った区分けをランダムに指定(指定した区域をさらに区分けする)
		divAfter_ = SafeRand(i);

		// いままで作った区分けをランダムに指定（指定した区域を更に分割する）
		if (_dng->mapDiv[divAfter_][0] - _dng->mapDiv[divAfter_][2] > _dng->mapDiv[divAfter_][1] - _dng->mapDiv[divAfter_][3])
		{
			count_ = RL_COUNT_X;
		}
		else
		{
			count_ = RL_COUNT_Y;
		}

		if (_dng->mapDiv[divAfter_][count_] - _dng->mapDiv[divAfter_][count_ + 2] < _dng->areaCountRand_ * 2 + MIN_DIVIDABLE_MARGIN)
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

		divided_ = false;

		// 分割可能範囲の算出
		size_t minRoomLen_ = (count_ == RL_COUNT_X) ? _dng->roomLengthMinX_ : _dng->roomLengthMinY_;
		size_t left_ = _dng->mapDiv[divAfter_][count_ + 2];
		size_t right_ = _dng->mapDiv[divAfter_][count_];
		size_t minDiv_ = left_ + minRoomLen_;
		size_t maxDiv_ = right_ - minRoomLen_;

		if (right_ - left_ + 1 >= minRoomLen_ * 2 + 1 && minDiv_ < maxDiv_)
		{
			size_t divPos_ = minDiv_ + SafeRand(maxDiv_ - minDiv_ + 1);

			// 新区画 i
			// +2は始点と終点の壁分
			_dng->mapDiv[i][count_] = divPos_;
			_dng->mapDiv[i][count_ + 2] = left_;
			_dng->mapDiv[i][abs(count_ - 1)] = _dng->mapDiv[divAfter_][abs(count_ - 1)];
			_dng->mapDiv[i][abs(count_ - 1) + 2] = _dng->mapDiv[divAfter_][abs(count_ - 1) + 2];

			// 既存区画 divAfter_ を更新
			_dng->mapDiv[divAfter_][count_ + 2] = divPos_;

			// 既存の道の接続を更新
			for (size_t j = 1; j < i; ++j)
			{
				if (_dng->mapRoad[j][0] == divAfter_)
				{
					_dng->mapRoad[j][0] = i;
				}
			}

			divided_ = true;
		}

		if (!divided_)
		{
			--i;
			++retry_;
		}
	}

	// 部屋を生成する処理
	for (size_t i = 0; i < _dng->mapDivCount; ++i)
	{
		// 区分け始点をマップX/Y始点へと代入
		size_t leftBorder = _dng->mapDiv[i][2];
		size_t rightBorder = _dng->mapDiv[i][0];
		size_t topBorder = _dng->mapDiv[i][3];
		size_t bottomBorder = _dng->mapDiv[i][1];

		_dng->mapRoom[i][2] = leftBorder + 1;   // X始点
		_dng->mapRoom[i][3] = topBorder + 1;    // Y始点

		// X方向の部屋サイズ決定（乱数を先に足して、それを区画内に clamp する）
		{
			size_t minW = _dng->roomLengthMinX_;
			size_t randW = _dng->roomLengthRandX_;
			size_t desiredWidth = minW + (randW > 0 ? SafeRand(randW) : 0);

			// 区画内で部屋が取れる最大幅（端の壁を考慮）
			size_t maxAvailableWidth = 0;
			if (rightBorder > _dng->mapRoom[i][2] + 1) {
				// rightBorder と mapRoom[][2] の差分。既存コードの意図に合わせて -1 をしている。
				maxAvailableWidth = rightBorder - (_dng->mapRoom[i][2] + 1);
			}
			// desiredWidth を区画内に収める（少なくとも 1 を確保）
			size_t actualWidth = desiredWidth;
			if (actualWidth > maxAvailableWidth) actualWidth = maxAvailableWidth;
			if (actualWidth < 1) actualWidth = (maxAvailableWidth >= 1 ? 1 : maxAvailableWidth); // maxAvailableWidth==0 の場合は 0 のまま

			// mapRoom の終点は区画の右端を超えないように clamp
			if (actualWidth > 0) {
				size_t tentativeXend = _dng->mapRoom[i][2] + actualWidth;
				// 右端 (rightBorder) の直前までに収める（壁分の余裕が必要なら -1 等を調整）
				if (tentativeXend >= rightBorder) tentativeXend = (rightBorder > 1 ? rightBorder - 1 : rightBorder);
				_dng->mapRoom[i][0] = tentativeXend;
			}
			else {
				// maxAvailableWidth == 0 の場合のフォールバック（可能な範囲内で最小サイズにする）
				_dng->mapRoom[i][0] = (std::min)(_dng->mapRoom[i][2] + 1, rightBorder > 0 ? rightBorder - 1 : rightBorder);
			}
		}

		// Y方向の部屋サイズ決定（Xと同様に処理）
		{
			size_t minH = _dng->roomLengthMinY_;
			size_t randH = _dng->roomLengthRandY_;
			size_t desiredHeight = minH + (randH > 0 ? SafeRand(randH) : 0);

			size_t maxAvailableHeight = 0;
			if (bottomBorder > _dng->mapRoom[i][3] + 1) {
				maxAvailableHeight = bottomBorder - (_dng->mapRoom[i][3] + 1);
			}
			size_t actualHeight = desiredHeight;
			if (actualHeight > maxAvailableHeight) actualHeight = maxAvailableHeight;
			if (actualHeight < 1) actualHeight = (maxAvailableHeight >= 1 ? 1 : maxAvailableHeight);

			if (actualHeight > 0) {
				size_t tentativeYend = _dng->mapRoom[i][3] + actualHeight;
				if (tentativeYend >= bottomBorder) tentativeYend = (bottomBorder > 1 ? bottomBorder - 1 : bottomBorder);
				_dng->mapRoom[i][1] = tentativeYend;
			}
			else {
				_dng->mapRoom[i][1] = (std::min)(_dng->mapRoom[i][3] + 1, bottomBorder > 0 ? bottomBorder - 1 : bottomBorder);
			}
		}

		// 幅や高さが極端に小さい場合の保険（区画の内側に収めるよう clamp）
		// +2 は最低でも2マス分の広さを確保するための既存意図を尊重しつつ、区画を越えないようにする
		if (_dng->mapRoom[i][0] <= _dng->mapRoom[i][2]) {
			size_t newXend = _dng->mapRoom[i][2] + 2;
			if (newXend >= rightBorder) {
				// 右端を越えるなら右端に詰める
				_dng->mapRoom[i][0] = (rightBorder > 1 ? rightBorder - 1 : rightBorder);
				// 必要なら始点を調整して幅を確保する（区画が極端に狭ければ開始位置を左に寄せる等のロジックを追加しても良い）
				if (_dng->mapRoom[i][0] <= _dng->mapRoom[i][2] && rightBorder > 2) {
					_dng->mapRoom[i][2] = rightBorder > 3 ? rightBorder - 3 : rightBorder - 2;
				}
			}
			else {
				_dng->mapRoom[i][0] = newXend;
			}
		}
		if (_dng->mapRoom[i][1] <= _dng->mapRoom[i][3]) {
			size_t newYend = _dng->mapRoom[i][3] + 2;
			if (newYend >= bottomBorder) {
				_dng->mapRoom[i][1] = (bottomBorder > 1 ? bottomBorder - 1 : bottomBorder);
				if (_dng->mapRoom[i][1] <= _dng->mapRoom[i][3] && bottomBorder > 2) {
					_dng->mapRoom[i][3] = bottomBorder > 3 ? bottomBorder - 3 : bottomBorder - 2;
				}
			}
			else {
				_dng->mapRoom[i][1] = newYend;
			}
		}

		// 部屋の描画
		size_t x0 = std::min<size_t>(_dng->mapRoom[i][2], _dng->mapRoom[i][0]);
		size_t x1 = std::max<size_t>(_dng->mapRoom[i][2], _dng->mapRoom[i][0]);
		size_t y0 = std::min<size_t>(_dng->mapRoom[i][3], _dng->mapRoom[i][1]);
		size_t y1 = std::max<size_t>(_dng->mapRoom[i][3], _dng->mapRoom[i][1]);
		for (size_t j = x0; j <= x1 && j < _maprl.size(); ++j)
		{
			for (size_t k = y0; k <= y1 && k < _maprl[j].size(); ++k)
			{
				_maprl[j][k].mapData = MAPCHIP_FLOOR;
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
		roomAfter_ = _dng->mapRoad[roomBefore_][0]; // 前の部屋に繋がる後の部屋を取得
		if (roomAfter_ == static_cast<size_t>(-1) || roomAfter_ >= _dng->mapDivCount) continue;

		// 部屋の中央座標を計算
		size_t beforeCenterX = (_dng->mapRoom[roomBefore_][2] + _dng->mapRoom[roomBefore_][0]) / 2;
		size_t beforeCenterY = (_dng->mapRoom[roomBefore_][3] + _dng->mapRoom[roomBefore_][1]) / 2;
		size_t afterCenterX = (_dng->mapRoom[roomAfter_][2] + _dng->mapRoom[roomAfter_][0]) / 2;
		size_t afterCenterY = (_dng->mapRoom[roomAfter_][3] + _dng->mapRoom[roomAfter_][1]) / 2;

		// L字型通路（まずX方向、次にY方向）
		// X方向
		if (beforeCenterX <= afterCenterX) {
			for (size_t x = beforeCenterX; x <= afterCenterX; ++x) {
				if (x < _maprl.size() && beforeCenterY < _maprl[x].size()) {
					_maprl[x][beforeCenterY].mapData = MAPCHIP_ROAD;
				}
			}
		}
		else {
			for (size_t x = afterCenterX; x <= beforeCenterX; ++x) {
				if (x < _maprl.size() && beforeCenterY < _maprl[x].size()) {
					_maprl[x][beforeCenterY].mapData = MAPCHIP_ROAD;
				}
			}
		}
		// Y方向
		if (beforeCenterY <= afterCenterY) {
			for (size_t y = beforeCenterY; y <= afterCenterY; ++y) {
				if (afterCenterX < _maprl.size() && y < _maprl[afterCenterX].size()) {
					_maprl[afterCenterX][y].mapData = MAPCHIP_ROAD;
				}
			}
		}
		else {
			for (size_t y = afterCenterY; y <= beforeCenterY; ++y) {
				if (afterCenterX < _maprl.size() && y < _maprl[afterCenterX].size()) {
					_maprl[afterCenterX][y].mapData = MAPCHIP_ROAD;
				}
			}
		}
	}

	return 0;
}