#include "ManaGauge.h"
#include "../Engine/Image.h"
#include <algorithm>
#include "../Engine/Direct3D.h"

namespace
{
	const float GAUGE_POS_X = 0.05f; // マナゲージのX位置の割合（画面幅に対する割合）
	const float GAUGE_POS_Y = 0.15f;  // マナゲージのY位置の割合（画面高さに対する割合）
	const float GAUGE_FRAME_OFFSET = 5.0f; // 枠のオフセット
}

ManaGauge::ManaGauge(GameObject* parent) : GameObject(parent), mana_(0.0f), maxMana_(0.0f), gaugeImage_(-1)
{
}

void ManaGauge::Initialize()
{
	gaugeImage_ = Image::Load("manaGauge.png");
	frameImage_ = Image::Load("gaugeFrame.png");
	assert(gaugeImage_ != -1);
	assert(frameImage_ != -1);

	posX_ = Direct3D::screenWidth_ * GAUGE_POS_X; // 画面左上から少し右にオフセット
	posY_ = Direct3D::screenHeight_ * GAUGE_POS_Y;  // 画面左上から少し下にオフセット

	SetDrawOrder(1);

	// UIなのでポーズ時も描画されるようにする
	SetIsUIObject(true);
}

void ManaGauge::Update()
{
	// 画面サイズが変わった時に位置を再計算する
	posX_ = Direct3D::screenWidth_ * GAUGE_POS_X; // 画面左上から少し右にオフセット
	posY_ = Direct3D::screenHeight_ * GAUGE_POS_Y;  // 画面左上から少し下にオフセット

	// サイズをリセット
	RECT rect = Image::GetRect(gaugeImage_);
	// mana/maxmana の0～１にする
	Image::ResetRect(gaugeImage_);
	rect = Image::GetRect(frameImage_);
	Image::ResetRect(frameImage_);
}

void ManaGauge::Draw()
{
	// まずフレームを描画
	Image::SetPositionPixels(frameImage_, posX_ - GAUGE_FRAME_OFFSET, posY_ - GAUGE_FRAME_OFFSET, false);
	Image::Draw(frameImage_);

	// マナ量に応じた割合を計算（安全に）
	if (maxMana_ <= 1e-6f)
	{
		ratio_ = 0.0f;
	}
	else 
	{
		ratio_ = mana_ / maxMana_;
	}

	// 0..1 にクランプ
	ratio_ = (std::clamp)(ratio_, 0.0f, 1.0f);

	// 元テクスチャのフルサイズを取得（GetRect は left,top,right,bottom で right は幅扱い）
	RECT rect = Image::GetRect(gaugeImage_);
	float srcW = static_cast<float>(rect.right - rect.left);
	float srcH = static_cast<float>(rect.bottom - rect.top);

	// 表示幅（ピクセル）
	int dispW = static_cast<int>(srcW * ratio_);
	// 0 幅だと見えないので、ratio>0 なら最小1ピクセル表示にする
	if (dispW == 0 && ratio_ > 0.0f) dispW = 1;

	// 1) まず SetRect で「切り出し幅」を設定する（テクスチャの表示領域）
	// 左上原点から右幅 dispW を取る実装に合わせる（x=0）
	Image::SetRect(gaugeImage_, 0, 0, dispW, static_cast<int>(srcH));

	// 2) 位置は外部で設定できるようにした posX_/posY_ を使う
	Image::SetPositionPixels(gaugeImage_, posX_, posY_, false);

	// 描画（スケールは使わない）
	Image::Draw(gaugeImage_);
}



void ManaGauge::Release()
{
}
