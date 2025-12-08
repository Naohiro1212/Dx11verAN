#include "../Source/TestScene.h"
#include "../Source/Player.h"
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
	//pWp = Instantiate<Weapon>(this);
	Instantiate<Plane>(this);
	Instantiate<Player>(this);
	Instantiate<testEnemy>(this);
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
