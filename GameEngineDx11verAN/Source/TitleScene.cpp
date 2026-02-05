#include "TitleScene.h"
#include "../Engine/Input.h"
#include "../Engine/SceneManager.h"
#include "../Engine/Image.h"
#include "../Engine/Debug.h"
#include "../Engine/Button.h"
#include "../Engine/Audio.h"
#include "../Source/CursorManager.h"

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
titleImage_(-1),
clickSoundHandle_(-1),
startDelay_(600),
endDelay_(600)
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
	assert(bgmHandle_ >= 0);
	Audio::SetVolume(bgmHandle_, 0.02f);
	Audio::Play(bgmHandle_);

	// クリック音読み込み
	clickSoundHandle_ = Audio::Load("Audio/click.wav");
	assert(clickSoundHandle_ >= 0);
	Audio::SetVolume(clickSoundHandle_, 0.5f);
}

void TitleScene::Update()
{
	XMFLOAT3 mousePos = Input::GetMousePosition();

	startButton_->Update();
	endButton_->Update();
	bool onStartButton = startButton_->GetOnButton();
	bool onEndButton = endButton_->GetOnButton();

	// クリックで遅延処理を予約
	if (onStartButton && Input::IsMouseButtonDown(0) && !pendingStart_ && !pendingEnd_)
	{
		Audio::Play(clickSoundHandle_);
		pendingStart_ = true;
		startTriggerTime_ = std::chrono::steady_clock::now();
	}

	if (onEndButton && Input::IsMouseButtonDown(0) && !pendingEnd_ && !pendingStart_)
	{
		Audio::Play(clickSoundHandle_);
		pendingEnd_ = true;
		endTriggerTime_ = std::chrono::steady_clock::now();
	}

	// 予約された処理の実行
	auto now = std::chrono::steady_clock::now();

	if (pendingStart_)
	{
		if(now - startTriggerTime_ >= startDelay_)
		{
			pendingStart_ = false;
			// BGM停止
			Audio::Stop(bgmHandle_);
			// シーン切り替え
			SceneManager* pSceneManager = dynamic_cast<SceneManager*>(GetParent());
			pSceneManager->ChangeScene(SCENE_ID_INFO);
		}
	}

	if(pendingEnd_)
	{
		if (now - endTriggerTime_ >= endDelay_)
		{
			pendingEnd_ = false;
			// BGM停止
			Audio::Stop(bgmHandle_);
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

	CursorManager* pCursorManager = dynamic_cast<CursorManager*>(FindObject("CursorManager"));
	pCursorManager->Draw();

	// 中心に配置（center = true）
	Image::SetPositionPixels(titleImage_, Direct3D::screenWidth_ * 0.5f, Direct3D::screenHeight_ * 0.5f, true);

	// 描画
	Image::Draw(titleImage_);
}

void TitleScene::Release()
{
}
