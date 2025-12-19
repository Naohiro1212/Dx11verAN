#include "TitleScene.h"
#include "../Engine/Input.h"
#include "../Engine/SceneManager.h"
#include "../Engine/Image.h"
#include "../Engine/Text.h"
#include "../Engine/Debug.h"

TitleScene::TitleScene(GameObject* parent) : GameObject(parent, "TitleScene"), onLogo(false)
{
}

void TitleScene::Initialize()
{
	// Image初期化
	Image::Initialize();

	// タイトルロゴの読み込み
	titleLogo_ = Image::Load("colormap.png");
	assert(titleLogo_ >= 0);

	Image::ResetRect(titleLogo_);
	Image::SetRect(titleLogo_, 400, 400, 110, 110);
	
	lTrans_.position_ = { 100.0f, 100.0f, 0.0f };
	Image::SetTransform(titleLogo_, lTrans_);

	pText_ = new Text();
	pText_->Initialize();
}

void TitleScene::Update()
{
	XMFLOAT3 mousePos = Input::GetMousePosition();

	// タイトルロゴの位置とサイズを取得
	rect = Image::GetRect(titleLogo_);
	float w = static_cast<float>(rect.right - rect.left);
	float h = static_cast<float>(rect.bottom - rect.top);
	float logoX = lTrans_.position_.x;
	float logoY = lTrans_.position_.y;

	// マウスがタイトルロゴの上にあるかどうかを判定
	if (mousePos.x >= logoX && mousePos.x <= logoX + w &&
		mousePos.y >= logoY && mousePos.y <= logoY + h)
	{
		onLogo = true;
	}
	else
	{
		onLogo = false;
	}

	if (onLogo)
	{
		// マウスがタイトルロゴの上にあるときの処理
		if (Input::IsMouseButtonDown(0))
		{
			// Enterキーが押されたらシーンを切り替える
			auto sceneManager = dynamic_cast<SceneManager*>(GetParent());
			if (sceneManager)
			{
				sceneManager->ChangeScene(SCENE_ID_TEST);
			}
		}
	}
}

void TitleScene::Draw()
{
	// スプライト描画
	if (onLogo)
	{
		Image::SetAlpha(titleLogo_, 200); // 半透明にする
		Image::Draw(titleLogo_);
	}
	else
	{
		Image::SetAlpha(titleLogo_, 255); // 通常の不透明度にする
		Image::Draw(titleLogo_);
	}
}

void TitleScene::Release()
{
}
