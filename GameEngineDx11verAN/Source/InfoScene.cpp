#include "InfoScene.h"
#include "../Engine/Image.h"
#include "../Engine/Button.h"
#include "../Engine/Input.h"
#include "../Engine/SceneManager.h"
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

	// ページ数設定
	currentPage = 0;
}

void InfoScene::Update()
{
	// 左クリックで次の情報ページへ
	if (Input::IsMouseButtonDown(0))
	{
		currentPage++;
	}
	// 最後のページならダンジョンシーンへ移動
	if (currentPage == 1)
	{
		// シーン切り替え
		SceneManager* pSceneManager = dynamic_cast<SceneManager*>(GetParent());
		pSceneManager->ChangeScene(SCENE_ID_TEST);
	}
}

void InfoScene::Draw()
{
	// 情報ページ画像描画
	Image::Draw(InfoImages_[currentPage]);

	if(currentPage == 0)
	{
		pInfoText_->Draw(Direct3D::screenWidth_ / 2 - 150.0f, Direct3D::screenHeight_ - 100.0f, "Press to Left Click to Next Page!");
	}
	else
	{
		pInfoText_->Draw(Direct3D::screenWidth_ / 2 - 150.0f, Direct3D::screenHeight_ - 130.0f, "Press to Right Click to Back Page!");
		pInfoText_->Draw(Direct3D::screenWidth_ / 2 - 150.0f, Direct3D::screenHeight_ - 100.0f, "Press to Left Click to Go Dungeon!");
	}
}

void InfoScene::Release()
{
}
