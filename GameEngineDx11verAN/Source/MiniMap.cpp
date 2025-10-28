#include "MiniMap.h"
#include "DungeonManager.h"
#include "EnemyBox.h"
#include "../Engine/Direct3D.h"
#include "../Engine/Camera.h"
#include "../Engine/Model.h"
#include "../Engine/Global.h"
#include "../Engine/Image.h"
#include <algorithm>

using namespace Direct3D;

namespace
{
	const float MINIMAP_SCALE = 256.0f; // ミニマップの表示サイズ（px）
	const float MINIMAP_MARGIN = 10.0f; // 右上マージン（px）
	const float MAPTILE_SIZE = 30.0f; // マップチップ1枚のサイズ
	const XMFLOAT3 MINIMAP_WALL_SCALE = { 23.5f, 15.0f, 15.0f }; // ミニマップ上のスケール
}

MiniMap::MiniMap(GameObject* parent)
	: GameObject(parent, "MiniMap"), wallModel_(-1), playerModel_(-1), frameImage_(-1),dungeonManager_(nullptr)
{
}

MiniMap::~MiniMap()
{
	dungeonManager_ = nullptr;
}

void MiniMap::Initialize()
{
	// 同階層から DungeonManager を取得
	dungeonManager_ = dynamic_cast<DungeonManager*>(FindObject("DungeonManager"));
	wallModel_ = Model::Load("Box.fbx");
	playerModel_ = Model::Load("Cube.fbx");
	frameImage_ = Image::Load("white.png");
}
void MiniMap::Update()
{
}

void MiniMap::Draw()
{
	if (!dungeonManager_)
	{
		return;
	}

	// マップデータがなければ描画しない
	const auto& map = dungeonManager_->GetMap();
	if (map.empty() || map[0].empty()) return;

	// 元のカメラとビューポートを保存
	XMFLOAT3 oldCamPos = Camera::GetPosition();
	XMFLOAT3 oldCamTarget = Camera::GetTarget();

	UINT oldCount = 1;
	D3D11_VIEWPORT oldViewport{};
	pContext_->RSGetViewports(&oldCount, &oldViewport);

	ID3D11DepthStencilState* oldDSS = nullptr;
	UINT oldStencilRef = 0;
	pContext_->OMGetDepthStencilState(&oldDSS, &oldStencilRef);

	// 画面右上のミニマップ矩形（画面ピクセル座標）
	const float mmX = static_cast<float>(screenWidth_) - MINIMAP_SCALE - MINIMAP_MARGIN;
	const float mmY = MINIMAP_MARGIN; // 不要な -50 は削除
	const float mmW = MINIMAP_SCALE;
	const float mmH = MINIMAP_SCALE;

	// 2Dの白背景を先に描く
	Image::SetRect(frameImage_, 0, 0, static_cast<int>(mmW), static_cast<int>(mmH));

	// 2D Transform（画面左上からのpx）
	Transform bgT{};
	bgT.position_ = { mmX, mmY, 0.0f }; 
	bgT.rotate_ = { 0, 0, 0 };
	bgT.scale_ = { 1, 1, 1 };      

	// 透明度を付けたいときは Image::SetAlpha(frameImage_, 200) などを呼ぶ（0-255）
	// Image::SetAlpha(frameImage_, 220);
	Image::SetTransform(frameImage_, bgT);
	Image::Draw(frameImage_);

	// 右上にミニマップ用ビューポートを設定
	D3D11_VIEWPORT mini{};
	mini.Width    = MINIMAP_SCALE;
	mini.Height   = MINIMAP_SCALE;
	mini.TopLeftX = static_cast<float>(screenWidth_) - MINIMAP_SCALE - MINIMAP_MARGIN;
	mini.TopLeftY = MINIMAP_MARGIN - 50.0f;
	mini.MinDepth = 0.0f;
	mini.MaxDepth = 1.0f;
	pContext_->RSSetViewports(1, &mini);

	// マップ全体が入るように俯瞰カメラに一時変更
	const float mapW = static_cast<float>(map.size()) * MAPTILE_SIZE;
	const float mapH = static_cast<float>(map[0].size()) * MAPTILE_SIZE;
	const XMFLOAT3 playerPos_ = dungeonManager_->GetPlayerPosition();
	const float maxDim = (std::max)(mapW, mapH);
	// 下に行き過ぎないように調整
	float h = (std::max)(maxDim * 0.6f, 80.0f);
	// 真上から見下ろす位置に設定
	const float eps = 1e-3f;
	Camera::SetPosition({playerPos_.x + eps, h, playerPos_.z });
	Camera::SetTarget({ playerPos_.x, 0.0f, playerPos_.z });
	Camera::Update();

	const XMFLOAT3 baseScale = MINIMAP_WALL_SCALE;
	// 壁チップ描画
	for (size_t i = 0; i < map.size(); ++i)
	{
		for (size_t j = 0; j < map[i].size(); ++j)
		{
			if (map[i][j].mapData == MAPCHIP_WALL)
			{
				Transform wallTransform_;
				wallTransform_.scale_ = baseScale;
				wallTransform_.rotate_ = {0,0,0};
				wallTransform_.position_ = { static_cast<float>(i) * MAPTILE_SIZE, 0.0f, static_cast<float>(j) * MAPTILE_SIZE };
				Model::SetTransform(wallModel_, wallTransform_);
				Model::Draw(wallModel_);
			}
		}
	}

	// プレイヤーマーカー（小さめ）
	plTransform_.scale_ = { baseScale.x * 0.5f, baseScale.y * 0.2f, baseScale.z * 0.5f };
	plTransform_.rotate_ = {0,0,0};
	plTransform_.position_ = dungeonManager_->GetPlayerPosition();
	Model::SetTransform(playerModel_, plTransform_);
	Model::Draw(playerModel_);
	


	// 敵マーカー（さらに小さめ）
	enTransform_.scale_ = { baseScale.x * 0.4f, baseScale.y * 0.2f, baseScale.z * 0.4f };
	enTransform_.rotate_ = {0,0,0};
	for (auto* enemy_ : dungeonManager_->GetEnemies())
	{
		if (!enemy_) continue;
		enTransform_.position_ = enemy_->GetPosition();
		Model::SetTransform(wallModel_, enTransform_);
		Model::Draw(wallModel_);
	}

	// 状態を復元
	Camera::SetPosition(oldCamPos);
	Camera::SetTarget(oldCamTarget);
	Camera::Update();
	if (oldDSS)
	{
		pContext_->OMSetDepthStencilState(oldDSS, oldStencilRef);
		oldDSS->Release();
	}
	pContext_->RSSetViewports(1, &oldViewport);
}

void MiniMap::Release()
{
}