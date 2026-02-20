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
#include "../Source/CursorManager.h"
#include <string>
#include <algorithm>

namespace
{
	// スコア（タイム）設定
	const int   MAX_TIME_SECONDS = 300; // 5分
	const int   POINTS_PER_SECOND = 10;  // 1秒あたりの減点（高いほど厳しい）
	const int   MAX_TIME_SCORE = MAX_TIME_SECONDS * POINTS_PER_SECOND;

	// ボタン位置オフセット
	const float BUTTON_Y_OFFSET = 200.0f;    // ボタンのY位置オフセット

	// テキスト描画位置オフセット（中心からの相対位置）
	const float SCORE_LABEL_OFFSET_X = -200.0f;
	const float SCORE_LABEL_OFFSET_Y = -160.0f;
	const float SCORE_VALUE_OFFSET_X = 70.0f;
	const float SCORE_VALUE_OFFSET_Y = -160.0f;

	const float TIME_LABEL_OFFSET_X = -200.0f;
	const float TIME_LABEL_OFFSET_Y = -110.0f;
	const float TIME_VALUE_OFFSET_X = 70.0f;
	const float TIME_VALUE_OFFSET_Y = -110.0f;

	const float BGM_VOLUME = 0.02f; // BGM音量
	const float CLICK_SOUND_VOLUME = 0.5f; // クリック音量

	const int SCORE_RANK_S = 2100;
	const int SCORE_RANK_A = 1200;

	const float RANK_LABEL_OFFSET_X = 150.0f;

	const float HALF_WINDOW_WIDTH = 0.5f;
	const int DELAY_CHANGE_SCENE = 600; // シーン切り替えの遅延時間（ミリ秒）
}

EndScene::EndScene(GameObject* parent)
    : GameObject(parent, "EndScene"),
      EndImage_(-1),
      pButton_(nullptr),
      bgTransform_(),
      bgmHandle_(-1),
      clickSoundHandle_(-1),
      pendingStart_(false),
      startTriggerTime_(),
      startDelay_(std::chrono::milliseconds(DELAY_CHANGE_SCENE))
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
	pButton_->SetButtonPosition(Direct3D::screenWidth_ * HALF_WINDOW_WIDTH, Direct3D::screenHeight_ * HALF_WINDOW_WIDTH + BUTTON_Y_OFFSET);

	// BGM読み込みと再生
	bgmHandle_ = Audio::Load("Audio/BGM_Title.wav", true);
	assert(bgmHandle_ >= 0);
	Audio::SetVolume(bgmHandle_, BGM_VOLUME);
	Audio::Play(bgmHandle_);

	// クリック音読み込み
	clickSoundHandle_ = Audio::Load("Audio/click.wav");
	assert(clickSoundHandle_ >= 0);
	Audio::SetVolume(clickSoundHandle_, CLICK_SOUND_VOLUME);

	pScoreText_ = new Text();
	pScoreText_->Initialize();
	pClearTimeText_ = new Text();
	pClearTimeText_->Initialize();

	// タイマー停止
	Timer::Stop();

	// タイム減点方式のスコア加算
	const int elapsedSec = static_cast<int>(Timer::GetSeconds());
	// 経過時間が長いほど減点、0～MAX_TIME_SCOREにクランプ
	const int timeScore = (std::clamp)(MAX_TIME_SCORE - elapsedSec * POINTS_PER_SECOND, 0, MAX_TIME_SCORE);

	// ScoreManager に反映
	ScoreManager::AddScore(timeScore);
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
	// カーソル描画
	CursorManager* pCursorManager = dynamic_cast<CursorManager*>(FindObject("CursorManager"));
	pCursorManager->Draw();

	// 画面を覆うスケール計算（cover）
	RECT rect = Image::GetRect(EndImage_);
	float w = (float)(rect.right - rect.left);
	float h = (float)(rect.bottom - rect.top);
	float scaleX = Direct3D::screenWidth_ / w;
	float scaleY = Direct3D::screenHeight_ / h;
	float scale = max(scaleX, scaleY);
	Image::SetSizePixels(EndImage_, w * scale, h * scale);

	// 中心に配置（center = true）
	Image::SetPositionPixels(EndImage_, Direct3D::screenWidth_ * HALF_WINDOW_WIDTH, Direct3D::screenHeight_ * HALF_WINDOW_WIDTH, true);

	// 描画
	Image::Draw(EndImage_);

	// スコア表示
	ScoreManager* pScoreManager = dynamic_cast<ScoreManager*>(FindObject("ScoreManager"));
	int score = pScoreManager->GetScore();

	// ラベルと値の位置（中心基準の相対オフセット）
	const float scoreLabelX = Direct3D::screenWidth_ * HALF_WINDOW_WIDTH + SCORE_LABEL_OFFSET_X;
	const float scoreLabelY = Direct3D::screenHeight_ * HALF_WINDOW_WIDTH + SCORE_LABEL_OFFSET_Y;
	const float scoreValueX = Direct3D::screenWidth_ * HALF_WINDOW_WIDTH + SCORE_VALUE_OFFSET_X;
	const float scoreValueY = Direct3D::screenHeight_ * HALF_WINDOW_WIDTH + SCORE_VALUE_OFFSET_Y;

	pScoreText_->Draw(scoreLabelX, scoreLabelY, "SCORE:");
	pScoreText_->Draw(scoreValueX, scoreValueY, score);

	if (score >= SCORE_RANK_S)
	{
		pScoreText_->Draw(scoreValueX + RANK_LABEL_OFFSET_X, scoreValueY, "RANK:S");
	}
	else if (score >= SCORE_RANK_A)
	{
		pScoreText_->Draw(scoreValueX + RANK_LABEL_OFFSET_X, scoreValueY, "RANK:A");
	}
	else
	{
		pScoreText_->Draw(scoreValueX + RANK_LABEL_OFFSET_X, scoreValueY, "RANK:B");
	}

	// クリアタイム表示
	std::string timeStr = Timer::ToString();
	const float timeLabelX = Direct3D::screenWidth_ * HALF_WINDOW_WIDTH + TIME_LABEL_OFFSET_X;
	const float timeLabelY = Direct3D::screenHeight_ * HALF_WINDOW_WIDTH + TIME_LABEL_OFFSET_Y;
	const float timeValueX = Direct3D::screenWidth_ * HALF_WINDOW_WIDTH + TIME_VALUE_OFFSET_X;
	const float timeValueY = Direct3D::screenHeight_ * HALF_WINDOW_WIDTH + TIME_VALUE_OFFSET_Y;

	pClearTimeText_->Draw(timeLabelX, timeLabelY, "CLEAR TIME:");
	pClearTimeText_->Draw(timeValueX, timeValueY, timeStr.c_str());
}

void EndScene::Release()
{
}
