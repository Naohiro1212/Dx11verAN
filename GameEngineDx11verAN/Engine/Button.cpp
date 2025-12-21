#include "Button.h"
#include "../Engine/Image.h"
#include "../Engine/Input.h"
#include "../Engine/Direct3D.h"

Button::Button(GameObject* parent) : GameObject(parent, "Button"), onButton_(false), buttonImage_(-1), center_(false)
{
}

void Button::Initialize()
{
	transform_.position_ = { 0.0f, 0.0f, 0.0f };

	if (buttonImage_ >= 0)
	{
		Image::ResetRect(buttonImage_);
	}
}


void Button::Update()
{
	XMFLOAT3 mousePos = Input::GetMousePosition();

	// 元画像のサイズ（ピクセル）
	RECT rect = Image::GetRect(buttonImage_);
	float srcW = static_cast<float>(rect.right - rect.left);
	float srcH = static_cast<float>(rect.bottom - rect.top);

	// 表示スケールを考慮したサイズ（Button::transform_.scale_ をピクセル倍率として使う）
	float scaledW = srcW * transform_.scale_.x;
	float scaledH = srcH * transform_.scale_.y;

	// transform_.position_ の扱いに合わせて左上座標を計算
	float left = center_ ? (transform_.position_.x - scaledW * 0.5f) : transform_.position_.x;
	float top = center_ ? (transform_.position_.y - scaledH * 0.5f) : transform_.position_.y;

	// マウスがボタンの上にあるかどうかを判定（クライアントピクセル座標で比較）
	if (mousePos.x >= left && mousePos.x <= left + scaledW &&
		mousePos.y >= top && mousePos.y <= top + scaledH)
	{
		onButton_ = true;
	}
	else
	{
		onButton_ = false;
	}
}

void Button::Draw()
{
	// ハンドル無効なら描画をスキップ
	if (buttonImage_ < 0)
	{
		return;
	}

	// 元画像のサイズ（ピクセル）
	RECT rect = Image::GetRect(buttonImage_);
	float srcW = static_cast<float>(rect.right - rect.left);
	float srcH = static_cast<float>(rect.bottom - rect.top);

	// transform_.scale_ をピクセル倍率として Image に伝える（常に設定しておくと判定と一致する）
	Image::SetSizePixels(buttonImage_, srcW * transform_.scale_.x, srcH * transform_.scale_.y);

	// 位置は左上基準 or 中心基準で Image に渡す
	Image::SetPositionPixels(buttonImage_, transform_.position_.x, transform_.position_.y, center_);

	if (onButton_)
	{
		Image::SetAlpha(buttonImage_, 100); // 半透明
		Image::Draw(buttonImage_);
	}
	else
	{
		Image::SetAlpha(buttonImage_, 255); // 不透明
		Image::Draw(buttonImage_);
	}
}

void Button::Release()
{
}

void Button::SetButtonScale(float width, float height)
{
	transform_.scale_ = { width, height, 1.0f };
}

void Button::ResetButtonSize()
{
	transform_.scale_ = { 1.0f, 1.0f, 1.0f };
}