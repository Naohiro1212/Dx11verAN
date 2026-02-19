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
	// ボタン位置は「画面中央からのオフセット」で管理
	// （16:9 でも 4:3 でも大体同じ見た目になる）
	const float START_BUTTON_OFFSET_Y = 80.0f;
	const float END_BUTTON_OFFSET_Y = 250.0f;


	const float BGM_VOLUME = 0.02f;
	const float CLICK_SOUND_VOLUME = 0.5f;

	const float HALF_WINDOW = 0.5f;
	const int   DELAY_CHANGE_SCENE = 600;
}

TitleScene::TitleScene(GameObject* parent) : GameObject(parent, "TitleScene"), 
startButton_(nullptr), 
endButton_(nullptr),
titleImage_(-1),
clickSoundHandle_(-1),
startDelay_(std::chrono::milliseconds(DELAY_CHANGE_SCENE)),
endDelay_(std::chrono::milliseconds(DELAY_CHANGE_SCENE))
{
}

void TitleScene::Initialize()
{
	// ボタン初期化
	startButton_ = Instantiate<Button>(this);
	startButton_->SetCenter(true);
	startButton_->SetButtonImage(Image::Load("startButton01.png"));

	endButton_ = Instantiate<Button>(this);
	endButton_->SetCenter(true);
	endButton_->SetButtonImage(Image::Load("endButton02.png"));

	// 背景画像読み込み
	titleImage_ = Image::Load("Title.jpg");
	assert(titleImage_ >= 0);
	Image::ResetRect(titleImage_);
	bgTransform_.position_ = { 0.0f, 0.0f, 0.0f };

	// BGM読み込みと再生
	bgmHandle_ = Audio::Load("Audio/BGM_Title.wav", true);
	assert(bgmHandle_ >= 0);
	Audio::SetVolume(bgmHandle_, BGM_VOLUME);
	Audio::Play(bgmHandle_);

	// クリック音読み込み
	clickSoundHandle_ = Audio::Load("Audio/click.wav");
	assert(clickSoundHandle_ >= 0);
	Audio::SetVolume(clickSoundHandle_, CLICK_SOUND_VOLUME);
}

void TitleScene::Update()
{
	// 毎フレームレイアウト更新する
	UpdateLayout();

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

	// 描画
	Image::Draw(titleImage_);
}

void TitleScene::Release()
{
}


void TitleScene::UpdateLayout()
{
	const float screenW = Direct3D::screenWidth_;
	const float screenH = Direct3D::screenHeight_;

	// ===== 背景イメージを画面全体にフィット（cover） =====
	RECT rect = Image::GetRect(titleImage_);
	float w = static_cast<float>(rect.right - rect.left);
	float h = static_cast<float>(rect.bottom - rect.top);

	float scaleX = screenW / w;
	float scaleY = screenH / h;
	float scale = (std::max)(scaleX, scaleY);

	Image::SetSizePixels(titleImage_, w * scale, h * scale);
	Image::SetPositionPixels(
		titleImage_,
		screenW * HALF_WINDOW,
		screenH * HALF_WINDOW,
		true  // center
	);

	// ===== ボタンの位置を画面中央基準で決定 =====
	const float centerX = screenW * HALF_WINDOW;
	const float centerY = screenH * HALF_WINDOW;

	// Startボタン：画面中央より少し下
	startButton_->SetButtonPosition(centerX, centerY + START_BUTTON_OFFSET_Y);

	// Endボタン：Startのさらに下
	endButton_->SetButtonPosition(centerX, centerY + END_BUTTON_OFFSET_Y);
}
