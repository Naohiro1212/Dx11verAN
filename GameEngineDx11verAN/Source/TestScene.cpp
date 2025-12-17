#include "../Source/TestScene.h"
#include "../Source/Player.h"
#include "../Source/DungeonManager.h"
#include "../Engine/Camera.h"
#include "../Engine/Input.h"
#include "../Source/Plane.h"
#include "../Source/testEnemy.h"
//コンストラクタ
TestScene::TestScene(GameObject * parent)
	: GameObject(parent, "TestScene")
{
}

//初期化
void TestScene::Initialize()
{	
	// effekseer初期設定

	Instantiate<DungeonManager>(this);
	Instantiate<Plane>(this);
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
