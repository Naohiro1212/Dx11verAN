#include "../Source/TestScene.h"
#include "../Source/Player.h"
#include "../Source/DungeonManager.h"
#include "../Source/Wall.h"
#include "../Engine/Camera.h"

//コンストラクタ
TestScene::TestScene(GameObject * parent)
	: GameObject(parent, "TestScene")
{
}

//初期化
void TestScene::Initialize()
{	
	// 上から見下ろすカメラ
	Camera::SetPosition(0.0f, 100.0f, -100.0f);

//	Instantiate <Player>(this);
	Instantiate <DungeonManager>(this);
}

//更新
void TestScene::Update()
{
}

//描画
void TestScene::Draw()
{
}

//開放
void TestScene::Release()
{
}
