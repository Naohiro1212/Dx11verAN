#include "healthGauge.h"
#include "../Engine/Image.h"
#include <algorithm>
#include "../Engine/Direct3D.h"

namespace
{
	const float GAUGE_POS_X = 100.0f; // ヘルスゲージのX位置
	const float GAUGE_POS_Y = 50.0f;  // ヘルスゲージのY位置
}

HealthGauge::HealthGauge(GameObject* parent) : GameObject(parent), health_(0.0f), maxHealth_(0.0f), gaugeImage_(-1)
{
}

void HealthGauge::Initialize()
{
	gaugeImage_ = Image::Load("healthGauge.png");
	assert(gaugeImage_ != -1);
	posX_ = GAUGE_POS_X; // 画面左上から少し右にオフセット
	posY_ = GAUGE_POS_Y;  // 画面左上から少し下にオフセット
	SetDrawOrder(1);
	// UIなのでポーズ時も描画されるようにする
	SetIsUIObject(true);
}

void HealthGauge::Update()
{
	RECT rect = Image::GetRect(gaugeImage_);
	// health/maxhealth の0～１にする
	Image::ResetRect(gaugeImage_);
}

void HealthGauge::Draw()
{
	// マナ量に応じた割合を計算
	if (maxHealth_ <= 1e-6f)
	{
		ratio_ = 0.0f;
	}
	else
	{
		ratio_ = health_ / maxHealth_;
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

void HealthGauge::Release()
{
}
