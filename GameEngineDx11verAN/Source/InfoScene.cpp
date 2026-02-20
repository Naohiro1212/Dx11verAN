#include "InfoScene.h"
#include "../Engine/Image.h"
#include "../Engine/Button.h"
#include "../Engine/Input.h"
#include "../Engine/SceneManager.h"
#include "../Source/CursorManager.h"
#include "../Engine/Text.h"
#include "../Engine/Audio.h"

InfoScene::InfoScene(GameObject* parent) : GameObject(parent, "InfoScene"), InfoImages_{ -1, -1 }
{
}

void InfoScene::Initialize()
{
	// 情報ページ画像読み込み
	InfoImages_[0] = Image::Load("InfoPage1.png");
	assert(InfoImages_[0] >= 0);
	InfoImages_[1] = Image::Load("InfoPage2.png");
	assert(InfoImages_[1] >= 0);

	pInfoText_ = new Text();
	pInfoText_->Initialize();

	// BGM読み込みと再生
	bgmHandle_ = Audio::Load("Audio/BGM_Title.wav", true);
	assert(bgmHandle_ >= 0);
	Audio::SetVolume(bgmHandle_, 0.02f);
	Audio::Play(bgmHandle_);

	// ページ数設定
	currentPage = 0;
}

void InfoScene::Update()
{
	// 現在の押下状態
	const bool mouseDown = Input::IsMouseButtonDown(0);
	// エッジ検出（押し始めた瞬間のみ true）
	const bool mouseClicked = mouseDown && !prevMouseLeftDown_;

	// 左クリックで次の情報ページへ（0 -> 1）
	if (mouseClicked && currentPage < 1)
	{
		currentPage++;
		// このクリックでは遷移させない（ここで終了）
		prevMouseLeftDown_ = mouseDown;
		return;
	}

	// 右クリックで前の情報ページへ（1 -> 0）
	if(Input::IsMouseButtonDown(1) && currentPage > 0)
	{
		currentPage--;
		// このクリックでは遷移させない（ここで終了）
		prevMouseLeftDown_ = mouseDown;
		return;
	}

	// 2枚目（currentPage == 1）になっていて、次のクリックでスタート
	if (currentPage == 1 && mouseClicked)
	{
		if (auto* pSceneManager = dynamic_cast<SceneManager*>(GetParent()))
		{
			pSceneManager->ChangeScene(SCENE_ID_TEST);
		}
	}

	// 前フレーム状態の更新
	prevMouseLeftDown_ = mouseDown;
}

void InfoScene::Draw()
{
	// 画面を覆うスケール計算（cover）
	for (int i = 0;i < 2;i++)
	{
		RECT rect = Image::GetRect(InfoImages_[i]);
		float w = (float)(rect.right - rect.left);
		float h = (float)(rect.bottom - rect.top);
		float scaleX = Direct3D::screenWidth_ / w;
		float scaleY = Direct3D::screenHeight_ / h;
		float scale = max(scaleX, scaleY);
		Image::SetSizePixels(InfoImages_[i], w * scale, h * scale);
	};

	// 情報ページ画像描画
	Image::Draw(InfoImages_[currentPage]);

	if(currentPage == 0)
	{
		pInfoText_->Draw(Direct3D::screenWidth_ / 2 - 150, Direct3D::screenHeight_ - 100, "Press to Left Click to Next Page!");
	}
	else
	{
		pInfoText_->Draw(Direct3D::screenWidth_ / 2 - 150, Direct3D::screenHeight_ - 130, "Press to Right Click to Back Page!");
		pInfoText_->Draw(Direct3D::screenWidth_ / 2 - 150, Direct3D::screenHeight_ - 100, "Press to Left Click to Go Dungeon!");
	}

	CursorManager* pCursorManager = dynamic_cast<CursorManager*>(FindObject("CursorManager"));
	pCursorManager->Draw();
}

void InfoScene::Release()
{
}
