#include "ManaGauge.h"
#include "../Engine/Image.h"

ManaGauge::ManaGauge(GameObject* parent) : GameObject(parent), mana_(0.0f), maxMana_(0.0f), gaugeImage_(-1)
{
}

void ManaGauge::Initialize()
{
	gaugeImage_ = Image::Load("manaGauge.png");
	assert(gaugeImage_ != -1);
}

void ManaGauge::Update()
{
	RECT rect = Image::GetRect(gaugeImage_);
	// mana/maxmana の0～１にする
	Image::ResetRect(gaugeImage_);
}

void ManaGauge::Draw()
{
	// マナ量に応じた割合を計算
	ratio_ = mana_ / maxMana_;
	// 元テクスチャのフルサイズを取得
	RECT rect = Image::GetRect(gaugeImage_);
	float srcW = static_cast<float>(rect.right - rect.left);
	float srcH = static_cast<float>(rect.bottom - rect.top);

	// 左上の表示位置（固定）
	const float left = 20.0f;
	const float top = 20.0f;

	// 表示幅（ピクセル）
	int dispW = static_cast<int>(srcW * ratio_);

	// 1) まず SetRect で「切り出し幅」を設定する（テクスチャの表示領域）
	Image::SetRect(gaugeImage_, 0, 0, dispW, static_cast<int>(srcH));

	// 2) 位置は左上固定にしたいので、center=falseで左上位置を指定
	Image::SetPositionPixels(gaugeImage_, left, top, false);

	// 描画（スケールは使わない）
	Image::Draw(gaugeImage_);
}


void ManaGauge::Release()
{
}
