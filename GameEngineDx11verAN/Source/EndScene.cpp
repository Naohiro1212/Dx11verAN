#include "EndScene.h"
#include "../Engine/Image.h"
#include "../Engine/SceneManager.h"
#include "../Engine/Button.h"
#include "../Engine/Input.h"
#include "../Engine/Text.h"
#include "../Engine/Direct3D.h"
#include "../Engine/ScoreManager.h"
#include "../Engine/Timer.h"
#include "../Engine/Audio.h"
#include <string>

EndScene::EndScene(GameObject* parent) : GameObject(parent, "EndScene"), EndImage_(-1), pButton_(nullptr), bgTransform_()
{
}

void EndScene::Initialize()
{

	// 背景画像読み込み
	EndImage_ = Image::Load("EndImage.png");
	assert(EndImage_ >= 0);
	Image::ResetRect(EndImage_);
	bgTransform_.position_ = { 0.0f, 0.0f, 0.0f };

	// ボタン初期化
	pButton_ = Instantiate<Button>(this);
	pButton_->SetCenter(true);
	pButton_->SetButtonImage(Image::Load("BackTitleButton.png"));
	pButton_->SetButtonPosition(Direct3D::screenWidth_ * 0.5f, Direct3D::screenHeight_ * 0.5f + 200.0f);

	// BGM読み込みと再生
	bgmHandle_ = Audio::Load("Audio/BGM_Title.wav", true);
	assert(bgmHandle_ >= 0);
	Audio::SetVolume(bgmHandle_, 0.02f);
	Audio::Play(bgmHandle_);

	// クリック音読み込み
	clickSoundHandle_ = Audio::Load("Audio/click.wav");
	assert(clickSoundHandle_ >= 0);
	Audio::SetVolume(clickSoundHandle_, 0.5f);

	pScoreText_ = new Text();
	pScoreText_->Initialize();
	pClearTimeText_ = new Text();
	pClearTimeText_->Initialize();

	// タイマー停止
	Timer::Stop();
}

void EndScene::Update()
{
	XMFLOAT3 mousePos = Input::GetMousePosition();
	pButton_->Update();
	bool onButton = pButton_->GetOnButton();

	if (onButton && Input::IsMouseButtonDown(0) && !pendingStart_)
	{
		Audio::Play(clickSoundHandle_);
		pendingStart_ = true;
		startTriggerTime_ = std::chrono::steady_clock::now();
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
			SceneManager* pSceneManager = dynamic_cast<SceneManager*>(FindObject("SceneManager"));
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
//	Image::Draw(EndImage_);

	// スコア表示
	ScoreManager* pScoreManager = dynamic_cast<ScoreManager*>(FindObject("ScoreManager"));
	int score = pScoreManager->GetScore();

	// 数字から文字に
	char scoreStr[32];
	std::to_string(score);
	pScoreText_->Draw(Direct3D::screenWidth_ * 0.5f - 200.0f, Direct3D::screenHeight_ * 0.5f - 130.0f, "SCORE:");
	pScoreText_->Draw(Direct3D::screenWidth_ * 0.5f + 70.0f, Direct3D::screenHeight_ * 0.5f - 130.0f, score);

	// クリアタイム表示
	std::string timeStr = Timer::ToString();
	pClearTimeText_->Draw(Direct3D::screenWidth_ * 0.5f - 200.0f, Direct3D::screenHeight_ * 0.5f - 80.0f, "CLEAR TIME:");
	pClearTimeText_->Draw(Direct3D::screenWidth_ * 0.5f + 70.0f, Direct3D::screenHeight_ * 0.5f - 80.0f, timeStr.c_str());
}

void EndScene::Release()
{
}
