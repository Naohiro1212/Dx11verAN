#include "TitleScene.h"
#include "../Engine/Input.h"
#include "../Engine/SceneManager.h"
#include "../Engine/Image.h"
#include "../Engine/Text.h"
#include "../Engine/Debug.h"
#include "../Engine/Button.h"
#include "../Engine/Image.h"

TitleScene::TitleScene(GameObject* parent) : GameObject(parent, "TitleScene"), pText_(nullptr), pButton_(nullptr), titleImage_(-1)
{
}

void TitleScene::Initialize()
{
	// Image初期化
	Image::Initialize();

	// ボタン初期化
	pButton_ = Instantiate<Button>(this);
	pButton_->SetCenter(true);
	pButton_->SetButtonImage(Image::Load("colormap.png"));
	pButton_->SetButtonPosition(Direct3D::screenWidth_ * 0.5f, Direct3D::screenHeight_ * 0.5f + 200.0f);

	// 背景画像読み込み
	titleImage_ = Image::Load("Title.jpg");
	assert(titleImage_ >= 0);
	Image::ResetRect(titleImage_);
	bgTransform_.position_ = { 0.0f, 0.0f, 0.0f };

	// テキスト初期化
	pText_ = new Text();
	pText_->Initialize();
}

void TitleScene::Update()
{
	XMFLOAT3 mousePos = Input::GetMousePosition();

	pButton_->Update();
	bool onButton = pButton_->GetOnButton();

	if (onButton)
	{
		if (Input::IsMouseButtonDown(0))
		{
			SceneManager* pSceneManager = dynamic_cast<SceneManager*>(GetParent());
			pSceneManager->ChangeScene(SCENE_ID_TEST);
		}
	}
}

void TitleScene::Draw()
{
	// 画面を覆うスケール計算（cover）
	RECT rect = Image::GetRect(titleImage_);
	float w = (float)(rect.right - rect.left);
	float h = (float)(rect.bottom - rect.top);
	float scaleX = Direct3D::screenWidth_ / w;
	float scaleY = Direct3D::screenHeight_ / h;
	float scale = max(scaleX, scaleY);
	Image::SetSizePixels(titleImage_, w * scale, h * scale);

	// 中心に配置（center = true）
	Image::SetPositionPixels(titleImage_, Direct3D::screenWidth_ * 0.5f, Direct3D::screenHeight_ * 0.5f, true);

	// 描画
	Image::Draw(titleImage_);

	// ボタン描画
	pButton_->Draw();

	pText_->Draw(Direct3D::screenWidth_ * 0.5f - 150.0f, Direct3D::screenHeight_ * 0.5f - 100.0f, "Press to Left Click!");
}

void TitleScene::Release()
{
}
