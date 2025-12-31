#include "../Source/TestScene.h"
#include "../Source/DungeonManager.h"
#include "../Engine/Input.h"
#include "../Source/Plane.h"
#include "../Source/Portal.h"
#include "../Source/Player.h"
#include "../Engine/GameObject.h"
#include "../Engine/SceneManager.h"
#include "../Source/ManaGauge.h"
#include "../Source/PausePanel.h"
#include "../Engine/Timer.h"

//コンストラクタ
TestScene::TestScene(GameObject * parent)
    : GameObject(parent, "TestScene"), isPaused_(false)
{
}

//初期化
void TestScene::Initialize()
{
    Instantiate<Plane>(this);
    dungeonManager_ = Instantiate<DungeonManager>(this);

    // ダンジョンマネージャーの初期化が終わった後にマナゲージを生成
    manaGauge_ = Instantiate<ManaGauge>(this);
    player_ = dynamic_cast<Player*>(FindObject("Player"));
    int maxMana_ = player_->GetMaxMana();
    manaGauge_->SetMana(0.0f);
    manaGauge_->SetMaxMana(maxMana_);

	// ポーズパネルの生成
	pausePanel_ = Instantiate<PausePanel>(this);

    Timer::Initialize();
    Timer::Start();
}

//更新
void TestScene::Update()
{
    // タイマー更新
    Timer::Update();

    // ESCキーで切り替え
    if (Input::IsKeyDown(DIK_ESCAPE))
    {
        isPaused_ = !isPaused_;
    }

    // ダンジョンが3階層以上になったらシーン移動
    if (dungeonManager_->GetNowFloor() >= 3)
    {
        SceneManager* pSceneManager = dynamic_cast<SceneManager*>(GetParent());
        pSceneManager->ChangeScene(SCENE_ID_END);
    }
    
	// プレイヤーがKillMeされていたらタイトルへ戻る   
    if(player_->IsDead())
    {
        SceneManager* pSceneManager = dynamic_cast<SceneManager*>(GetParent());
        pSceneManager->ChangeScene(SCENE_ID_END);
	}

    // ポーズパネルで再開ボタンが押されたらポーズ解除
    if (isPaused_ && pausePanel_->IsResumeButtonOn() && Input::IsMouseButtonDown(0))
    {
		isPaused_ = false;
    }

	// ポーズパネルでタイトルへ戻るボタンが押されたらタイトルシーンへ
    if (isPaused_ && pausePanel_->IsBackTitleButtonOn() && Input::IsMouseButtonDown(0))
    {
		SceneManager* pSceneManager = dynamic_cast<SceneManager*>(GetParent());
		pSceneManager->ChangeScene(SCENE_ID_TITLE);
    }

	// ポーズ状態の反映
    if (isPaused_)
    {
        pausePanel_->SetPaused(true);
        this->StopAllUpdate();      // 子オブジェクトだけ止める
    }
    else
    {
        pausePanel_->SetPaused(false);
        this->ResumeAllUpdate();    // 子オブジェクトだけ再開
    }

    // マナゲージの更新
    // シーンの子オブジェクトの最後にプッシュする
    manaGauge_->SetMana(player_->GetMana());
}

//描画
void TestScene::Draw()
{
}

//開放
void TestScene::Release()
{
}
