#include "TitleScene.h"
#include "../Engine/Input.h"
#include "../Engine/SceneManager.h"
#include "../Engine/Image.h"
#include "../Engine/Debug.h"
#include "../Engine/Button.h"
#include "../Engine/Audio.h"

namespace
{
	const float START_BUTTON_POS_X = 960.0f;
	const float START_BUTTON_POS_Y = 740.0f;
	const float END_BUTTON_POS_X = 960.0f;
	const float END_BUTTON_POS_Y = 890.0f;
}

TitleScene::TitleScene(GameObject* parent) : GameObject(parent, "TitleScene"), 
startButton_(nullptr), 
endButton_(nullptr),
titleImage_(-1)
{
}

void TitleScene::Initialize()
{
	// ボタン初期化
	startButton_ = Instantiate<Button>(this);
	startButton_->SetCenter(true);
	startButton_->SetButtonImage(Image::Load("startButton01.png"));
	startButton_->SetButtonPosition(START_BUTTON_POS_X, START_BUTTON_POS_Y);
	endButton_ = Instantiate<Button>(this);
	endButton_->SetCenter(true);
	endButton_->SetButtonImage(Image::Load("endButton02.png"));
	endButton_->SetButtonPosition(END_BUTTON_POS_X, END_BUTTON_POS_Y);

	// 背景画像読み込み
	titleImage_ = Image::Load("Title.jpg");
	assert(titleImage_ >= 0);
	Image::ResetRect(titleImage_);
	bgTransform_.position_ = { 0.0f, 0.0f, 0.0f };

	// BGM読み込みと再生
	bgmHandle_ = Audio::Load("Audio/BGM_Title.wav", true);
	Audio::SetVolume(bgmHandle_, 0.02f);
	Audio::Play(bgmHandle_);
}

void TitleScene::Update()
{
	XMFLOAT3 mousePos = Input::GetMousePosition();

	startButton_->Update();
	endButton_->Update();
	bool onStartButton = startButton_->GetOnButton();
	bool onEndButton = endButton_->GetOnButton();

	if (onStartButton)
	{
		if (Input::IsMouseButtonDown(0))
		{
			// BGM停止
			Audio::Stop(bgmHandle_);
			// シーン切り替え
			SceneManager* pSceneManager = dynamic_cast<SceneManager*>(GetParent());
			pSceneManager->ChangeScene(SCENE_ID_INFO);
		}
	}
	else if (onEndButton)
	{
		if(Input::IsMouseButtonDown(0))
		{
			// アプリ終了
			PostQuitMessage(0);
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
}

void TitleScene::Release()
{
}
