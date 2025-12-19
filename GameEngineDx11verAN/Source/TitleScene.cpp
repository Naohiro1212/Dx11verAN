#include "TitleScene.h"
#include "../Engine/Input.h"
#include "../Engine/SceneManager.h"
#include "../Engine/Sprite.h"
#include "../Engine/Debug.h"

TitleScene::TitleScene(GameObject* parent) : GameObject(parent, "TitleScene"), onLogo(false)
{
}

void TitleScene::Initialize()
{
	sprite = new Sprite();

	// タイトルロゴの読み込み
	titleLogo_ = sprite->Load("colormap.png");
	assert(titleLogo_ >= 0);

	// 画像サイズを取得して矩形にセット
	XMFLOAT3 texSize = sprite->GetTextureSize();
	rect.left = 0;
	rect.top = 0;
	rect.right = static_cast<LONG>(texSize.x);
	rect.bottom = static_cast<LONG>(texSize.y);

	sTrans_.position_ = { 100.0f, 100.0f, 0.0f };
}

void TitleScene::Update()
{
	XMFLOAT3 mousePos = Input::GetMousePosition();
	Debug::Log(mousePos.x);
	Debug::Log(mousePos.y, true);

	float w = static_cast<float>(rect.right - rect.left);
	float h = static_cast<float>(rect.bottom - rect.top);
	float cx = sTrans_.position_.x;
	float cy = sTrans_.position_.y;
	Debug::Log(w);
	Debug::Log(h, true);


	// マウスがタイトルロゴの上にあるかどうか判定
	onLogo = (mousePos.x >= cx) && (mousePos.x <= cy + w) &&
		(mousePos.y >= cx) && (mousePos.y <= cy + h);

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
		sprite->Draw(sTrans_,rect, 0.5f); // 半透明で描画
	}
	else
	{
		sprite->Draw(sTrans_,rect, 1.0f);
	}
}

void TitleScene::Release()
{
}
