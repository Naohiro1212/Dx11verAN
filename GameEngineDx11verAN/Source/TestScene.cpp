#include "../Source/TestScene.h"
#include "../Source/DungeonManager.h"
#include "../Engine/Input.h"
#include "../Source/Plane.h"
#include "../Source/Portal.h"
#include "../Engine/GameObject.h"

//コンストラクタ
TestScene::TestScene(GameObject * parent)
	: GameObject(parent, "TestScene"), isPaused_(false)
{
}

//初期化
void TestScene::Initialize()
{	
	Instantiate<Plane>(this);
	Instantiate<DungeonManager>(this);
    Instantiate<Portal>(this);
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
}

//描画
void TestScene::Draw()
{
}

//開放
void TestScene::Release()
{
}
