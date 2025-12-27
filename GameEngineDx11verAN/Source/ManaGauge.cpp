#include "ManaGauge.h"
#include "../Engine/Image.h"
#include <algorithm>

ManaGauge::ManaGauge(GameObject* parent) : GameObject(parent), mana_(0.0f), maxMana_(0.0f), gaugeImage_(-1)
{
}

void ManaGauge::Initialize()
{
	gaugeImage_ = Image::Load("manaGauge.png");
	assert(gaugeImage_ != -1);


	posX_ = 100.0f; // 画面左上から少し右にオフセット
	posY_ = 50.0f;  // 画面左上から少し下にオフセット
}

void ManaGauge::Update()
{
	RECT rect = Image::GetRect(gaugeImage_);
	// mana/maxmana の0～１にする
	Image::ResetRect(gaugeImage_);
}

void ManaGauge::Draw()
{
	// マナ量に応じた割合を計算（安全に）
	if (maxMana_ <= 1e-6f) {
		ratio_ = 0.0f;
	}
	else {
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
