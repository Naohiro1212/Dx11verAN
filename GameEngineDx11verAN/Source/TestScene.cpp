#include "../Source/TestScene.h"
#include "../Source/DungeonManager.h"
#include "../Engine/Input.h"
#include "../Source/Plane.h"
#include "../Source/Portal.h"
#include "../Source/Player.h"
#include "../Engine/GameObject.h"
#include "../Engine/SceneManager.h"
#include "../Source/ManaGauge.h"

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
}

//更新
void TestScene::Update()
{
    // ESCキーで切り替え
    if (Input::IsKeyDown(DIK_ESCAPE))
    {
        isPaused_ = !isPaused_;
        if (isPaused_) {
            this->StopAllUpdate();      // 子オブジェクトだけ止める
        }
        else {
            this->ResumeAllUpdate();    // 子オブジェクトだけ再開
        }
    }

	// ダンジョンが3階層以上になったらシーン移動
	if (dungeonManager_->GetNowFloor() >= 3)
	{
		SceneManager* pSceneManager = dynamic_cast<SceneManager*>(GetParent());
		pSceneManager->ChangeScene(SCENE_ID_TITLE);
	}

	// マナゲージの更新
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
