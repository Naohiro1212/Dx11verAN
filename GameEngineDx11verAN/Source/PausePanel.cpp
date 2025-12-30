#include "PausePanel.h"
#include "../Engine/Image.h"
#include "../Engine/Direct3D.h"
#include <assert.h>

PausePanel::PausePanel(GameObject* parent) : GameObject(parent), panelImage_(-1), nowPaused_(false)
{
}

void PausePanel::Initialize()
{
	panelImage_ = Image::Load("PausePanel.png");
	assert(panelImage_ != -1);

	// 読み込み直後に矩形を初期化しておく
	Image::ResetRect(panelImage_);

	// 画像サイズを取得して中央に一度だけ配置する（center=true を使う）
	RECT rect = Image::GetRect(panelImage_);
	float w = (float)(rect.right - rect.left);
	float h = (float)(rect.bottom - rect.top);

	// center=true にすることで SetPositionPixels に x,y を画面中心として渡せる
	Image::SetPositionPixels(panelImage_, Direct3D::screenWidth_ * 0.5f, Direct3D::screenHeight_ * 0.5f, true);

	nowPaused_ = false;

	SetDrawOrder(1);
	// UIなのでポーズ時も描画されるようにする
	SetIsUIObject(true);
}

void PausePanel::Update()
{
	// 特に毎フレームの矩形調整は不要にした
}

void PausePanel::Draw()
{
	// デバッグ確認のため、一時的に常に描画する（動作確認後に nowPaused_ 条件に戻してください）
	// if(nowPaused_)
	{
		Image::Draw(panelImage_);
	}
}

void PausePanel::Release()
{
}