#include "MiniMap.h"
#include "DungeonManager.h"
#include "EnemyBox.h"
#include "../Engine/Direct3D.h"
#include "../Engine/Camera.h"
#include "../Engine/Model.h"
#include "../Engine/Global.h"
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
	: GameObject(parent, "MiniMap"), wallModel_(-1), dungeonManager_(nullptr)
	, noDepthState_(nullptr)
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

	// 深度テスト向こうのデプス・ステンシルステートを作成
	D3D11_DEPTH_STENCIL_DESC dcDesc{};
	dcDesc.DepthEnable = FALSE; // 深度テスト無効
	dcDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dcDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	dcDesc.StencilEnable = FALSE;
	if (pDevice_)
	{
		pDevice_->CreateDepthStencilState(&dcDesc, &noDepthState_);
	}

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

	// 右上にミニマップ用ビューポートを設定
	D3D11_VIEWPORT mini{};
	mini.Width    = MINIMAP_SCALE;
	mini.Height   = MINIMAP_SCALE;
	mini.TopLeftX = static_cast<float>(screenWidth_) - MINIMAP_SCALE - MINIMAP_MARGIN;
	mini.TopLeftY = MINIMAP_MARGIN - 50.0f;
	mini.MinDepth = 0.0f;
	mini.MaxDepth = 1.0f;
	pContext_->RSSetViewports(1, &mini);

	// オーバーレイとして深度書き込みを無効化
//	SetDepthBafferWriteEnable(false);
	if (noDepthState_)
	{
		pContext_->OMSetDepthStencilState(noDepthState_, 0);
	}

	// マップ全体が入るように俯瞰カメラに一時変更
	const float tileSize = MAPTILE_SIZE;
	const float mapW = static_cast<float>(map.size()) * tileSize;
	const float mapH = static_cast<float>(map[0].size()) * tileSize;
	const XMFLOAT3 playerPos_ = dungeonManager_->GetPlayerPosition();
	const float maxDim = (std::max)(mapW, mapH);
	float h = (std::max)(mapW, mapH) * 0.6f;
	h = (std::max)(h, 80.0f); // 低くしすぎて地面下にもぐらないように制限を付ける
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
				wallTransform_.position_ = { static_cast<float>(i) * tileSize, 0.0f, static_cast<float>(j) * tileSize };
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
//	SetDepthBafferWriteEnable(true);
	if (oldDSS)
	{
		pContext_->OMSetDepthStencilState(oldDSS, oldStencilRef);
		oldDSS->Release();
	}
	pContext_->RSSetViewports(1, &oldViewport);
}

void MiniMap::Release()
{
	SAFE_RELEASE(noDepthState_);
}