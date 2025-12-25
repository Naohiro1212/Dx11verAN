#include "EndScene.h"
#include "../Engine/Image.h"
#include "../Engine/SceneManager.h"
#include "../Engine/Button.h"
#include "../Engine/Input.h"
#include "../Engine/Direct3D.h"
#include "../Engine/ScoreManager.h"

EndScene::EndScene(GameObject* parent) : GameObject(parent, "EndScene"), EndImage_(-1), pButton_(nullptr)
{
}

void EndScene::Initialize()
{
	// Image初期化
	Image::Initialize();

	// 背景画像読み込み
	EndImage_ = Image::Load("EndImage.png");
	assert(EndImage_ >= 0);
	Image::ResetRect(EndImage_);
	bgTransform_.position_ = { 0.0f, 0.0f, 0.0f };

	// ボタン初期化
	pButton_ = Instantiate<Button>(this);
	pButton_->SetCenter(true);
	pButton_->SetButtonImage(Image::Load("colormap.png"));
	pButton_->SetButtonPosition(Direct3D::screenWidth_ * 0.5f, Direct3D::screenHeight_ * 0.5f + 200.0f);
}

void EndScene::Update()
{
	XMFLOAT3 mousePos = Input::GetMousePosition();
	pButton_->Update();
	bool onButton = pButton_->GetOnButton();

	if(onButton)
	{
		if (Input::IsMouseButtonDown(0))
		{
			SceneManager* pSceneManager = dynamic_cast<SceneManager*>(GetParent());
			pSceneManager->ChangeScene(SCENE_ID_TITLE);
		}
	}
}

void EndScene::Draw()
{
	// 画面を覆うスケール計算（cover）
	RECT rect = Image::GetRect(EndImage_);
	float w = (float)(rect.right - rect.left);
	float h = (float)(rect.bottom - rect.top);
	float scaleX = Direct3D::screenWidth_ / w;
	float scaleY = Direct3D::screenHeight_ / h;
	float scale = max(scaleX, scaleY);
	Image::SetSizePixels(EndImage_, w * scale, h * scale);

	// 中心に配置（center = true）
	Image::SetPositionPixels(EndImage_, Direct3D::screenWidth_ * 0.5f, Direct3D::screenHeight_ * 0.5f, true);

	// 描画
	Image::Draw(EndImage_);

	// ボタン描画
	pButton_->Draw();
}

void EndScene::Release()
{
}
