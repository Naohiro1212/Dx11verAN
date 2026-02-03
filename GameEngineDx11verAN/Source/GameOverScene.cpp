#include "GameOverScene.h"
#include "../Engine/Image.h"
#include "../Engine/SceneManager.h"
#include "../Engine/Button.h"
#include "../Engine/Audio.h"
#include "../Engine/Timer.h"
#include "../Engine/Input.h"

GameOverScene::GameOverScene(GameObject* parent)
	: GameObject(parent, "GameOverScene"),
	gameOverImage_(-1),
	bgTransform_(),
	bgmHandle_(-1),
	pendingStart_(false),
	restartTriggerTime_(),
	restartDelay_(std::chrono::milliseconds(600))
{
}

void GameOverScene::Initialize()
{
	// ゲームオーバー画像読み込み
	gameOverImage_ = Image::Load("GameOverScene.png");
	assert(gameOverImage_ >= 0);
	Image::ResetRect(gameOverImage_);
	bgTransform_.position_ = { 0.0f, 0.0f, 0.0f };

	// ボタン初期化
	pButton_ = Instantiate<Button>(this);
	pButton_->SetCenter(true);
	pButton_->SetButtonImage(Image::Load("BackTitleButton.png"));
	pButton_->SetButtonPosition(Direct3D::screenWidth_ * 0.5f, Direct3D::screenHeight_ * 0.5f + 200.0f);

	// BGM読み込みと再生
	// 後で正式なBGMに差し替え
	bgmHandle_ = Audio::Load("Audio/BGM_Title.wav", true);
	assert(bgmHandle_ >= 0);
	Audio::SetVolume(bgmHandle_, 0.02f);
	Audio::Play(bgmHandle_);

	// クリック音読み込み
	clickSoundHandle_ = Audio::Load("Audio/click.wav");
	assert(clickSoundHandle_ >= 0);
	Audio::SetVolume(clickSoundHandle_, 0.5f);

	Timer::Stop();
}

void GameOverScene::Update()
{
	XMFLOAT3 mousePos = Input::GetMousePosition();
	pButton_->Update();
	bool onButton = pButton_->GetOnButton();

	if (onButton && Input::IsMouseButtonDown(0) && !pendingStart_)
	{
		Audio::Play(clickSoundHandle_);
		pendingStart_ = true;
		restartTriggerTime_ = std::chrono::steady_clock::now();
	}

	// 予約された処理の実行
	auto now = std::chrono::steady_clock::now();

	if (pendingStart_)
	{
		if (now - restartTriggerTime_ >= restartDelay_)
		{
			pendingStart_ = false;
			// BGM停止
			Audio::Stop(bgmHandle_);
			// シーン切り替え
			SceneManager* pSceneManager = dynamic_cast<SceneManager*>(GetParent());
			pSceneManager->ChangeScene(SCENE_ID_TITLE);
		}
	}
}

void GameOverScene::Draw()
{
	// 画面を覆うスケール計算（cover）
	RECT rect = Image::GetRect(gameOverImage_);
	float w = (float)(rect.right - rect.left);
	float h = (float)(rect.bottom - rect.top);
	float scaleX = Direct3D::screenWidth_ / w;
	float scaleY = Direct3D::screenHeight_ / h;
	float scale = max(scaleX, scaleY);
	Image::SetSizePixels(gameOverImage_, w * scale, h * scale);

	// 中心に配置（center = true）
	Image::SetPositionPixels(gameOverImage_, Direct3D::screenWidth_ * 0.5f, Direct3D::screenHeight_ * 0.5f, true);
	Image::Draw(gameOverImage_);


}

void GameOverScene::Release()
{
}
