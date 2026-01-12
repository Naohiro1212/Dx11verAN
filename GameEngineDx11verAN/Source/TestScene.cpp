#include "../Source/TestScene.h"
#include "../Source/DungeonManager.h"
#include "../Engine/Input.h"
#include "../Source/Plane.h"
#include "../Source/Portal.h"
#include "../Source/Player.h"
#include "../Engine/GameObject.h"
#include "../Engine/SceneManager.h"
#include "../Source/ManaGauge.h"
#include "../Source/healthGauge.h"
#include "../Source/PausePanel.h"
#include "../Engine/Timer.h"
#include "../Source/PopUpDamage.h" 
#include "../Source/ObjectiveText.h"

//コンストラクタ
TestScene::TestScene(GameObject * parent)
    : GameObject(parent, "TestScene"), isPaused_(false), objectiveText_(nullptr)
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

    // ヘルスゲージの生成
	healthGauge_ = Instantiate<HealthGauge>(this);
	int maxHealth_ = player_->GetMaxHealth();
	healthGauge_->SetHealth(0.0f);
	healthGauge_->SetMaxHealth(maxHealth_);

	// ポーズパネルの生成
	pausePanel_ = Instantiate<PausePanel>(this);

    Timer::Initialize();
    Timer::Start();

    // ポップアップダメージのモデル読み込み
    PopUpDamage::PreLoadDigitModels();

	objectiveText_ = Instantiate<ObjectiveText>(this);
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
    
	// プレイヤーが死亡したらエンドシーンへ   
    if(player_->GetDeathTimer() >= 5.0f)
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

    // ゲージの更新
    // シーンの子オブジェクトの最後にプッシュする(SetDrawOrderの順番)
    manaGauge_->SetMana(player_->GetMana());
	healthGauge_->SetHealth(player_->GetHealth());
}

//描画
void TestScene::Draw()
{
}

//開放
void TestScene::Release()
{
}
