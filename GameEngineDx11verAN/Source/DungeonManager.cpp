#include "DungeonManager.h"
#include "DungeonGenerator.h"
#include "../Engine/Model.h"
#include <algorithm> 
#include <vector>
#include "Wall.h"

namespace
{
	const size_t AREACOUNT_MIN = 4; // マップの区分け最小数
	const size_t AREACOUNT_RAND = 2; // マップの区分け数加算
	const size_t ROOMLENGTH_MIN_X = 5; // 部屋のX座標の最小サイズ
	const size_t ROOMLENGTH_MIN_Y = 5; // 部屋のY座標の最小サイズ
	const size_t ROOMLENGTH_RAND_X = 1; // 部屋のX座標のサイズ加算
	const size_t ROOMLENGTH_RAND_Y = 1; // 部屋のY座標のサイズ加算
	const size_t MAPX_RLk = 32; //マップ縦サイズ
	const size_t MAPY_RLk = 32;   //マップ横サイズ
}

DungeonManager::DungeonManager(GameObject* _parent)
	: dungeonGenerator_(nullptr), maxWall_(0), GameObject(_parent, "DungeonManager")
{
	dungeonMapInfo_ = new DungeonMap_Info
	{
		AREACOUNT_MIN,
		AREACOUNT_RAND,
		ROOMLENGTH_MIN_X,
		ROOMLENGTH_MIN_Y,
		ROOMLENGTH_RAND_X,
		ROOMLENGTH_RAND_Y
	};

	// マップのデータを表す整数値を持つベクター
	maprl = std::vector<std::vector<MapData_RL>>(MAPX_RLk, std::vector<MapData_RL>(MAPY_RLk, MAPCHIP_WALL));
}

DungeonManager::~DungeonManager()
{
}

void DungeonManager::Initialize()
{
	wallModel_ = Model::Load("Box.fbx");

	dungeonGenerator_ = new DungeonGenerator();
	dungeonGenerator_->Initialize();
	dungeonGenerator_->GenerateDungeon(dungeonMapInfo_, maprl);

	//maxWall_ = MAPX_RLk * MAPY_RLk;
	//// 壁総数を数える
	//size_t wallTotal_ = 0;
	//for(size_t i = 0; i < MAPX_RLk; ++i)
	//{
	//	for (size_t j = 0; j < MAPY_RLk; ++j)
	//	{
	//		// 1なら壁、0なら床
	//		if (maprl[i][j].mapData == MAPCHIP_WALL)
	//		{
	//			++wallTotal_;
	//		}
	//	}
	//}

	//// プールサイズwallTotal_とmaxWall_の小さい方を採用
	//size_t poolSize_ = (std::min)(maxWall_, wallTotal_);

	//// Wallプールの生成
	//if (wallPool_.empty())
	//{
	//	wallPool_.reserve(poolSize_);
	//	for (size_t n = 0; n < poolSize_; ++n)
	//	{
	//		Wall* w = Instantiate<Wall>(this, XMFLOAT3(0.0f,0.0f,0.0f));
	//		w->Invisible(); // 最初は非表示
	//		wallPool_.push_back(w);
	//	}
	//}
	//else
	//{
	//	// 既にあるプールが小さければ拡張
	//	if (wallPool_.size() < poolSize_)
	//	{
	//		for (size_t n = wallPool_.size(); n < poolSize_; ++n)
	//		{
	//			Wall* w = Instantiate<Wall>(this, XMFLOAT3(0.0f,0.0f,0.0f));
	//			w->Invisible(); // 最初は非表示
	//			wallPool_.push_back(w);
	//		}
	//	}
	//}

	//// プールから位置を割り当てて表示
	//size_t idx = 0;
	//for (size_t i = 0; i < MAPX_RLk && idx < wallPool_.size(); ++i)
	//{
	//	for (size_t j = 0; j < MAPY_RLk && idx < wallPool_.size(); ++j)
	//	{
	//		if (maprl[i][j].mapData == MAPCHIP_WALL)
	//		{
	//			Wall* w = wallPool_[idx++];
	//			// 壁の大きさを設定
	//			w->SetPosition(XMFLOAT3(static_cast<float>(i) * 30.0f, 0.0f, static_cast<float>(j) * 30.0f));
	//			w->Visible();
	//		}
	//	}
	//}

	//// 残りは非表示に
	/*for(; idx < wallPool_.size(); ++idx)
	{
		wallPool_[idx]->Invisible();
	}*/
}

void DungeonManager::Update()
{
}

void DungeonManager::Draw()
{
	for (size_t i = 0; i < MAPX_RLk; ++i)
	{
		for (size_t j = 0;j < MAPY_RLk; ++j)
		{
			Transform mapTransform_;
			mapTransform_.position_ = { static_cast<float>(i) * 30.0f, 0.0f, static_cast<float>(j) * 30.0f };
			switch (maprl[i][j].mapData)
			{
			case MAPCHIP_WALL:
				mapTransform_.scale_ = { 10.0f, 10.0f, 10.0f };
				Model::SetTransform(wallModel_, mapTransform_);
				Model::Draw(wallModel_);
			}
		}
	}
}

void DungeonManager::Release()
{
}
