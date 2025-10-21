#include "../Source/TestScene.h"
#include "../Source/Player.h"
#include "../Source/DungeonManager.h"
#include "../Engine/Camera.h"
#include "../Engine/Input.h"

//コンストラクタ
TestScene::TestScene(GameObject * parent)
	: GameObject(parent, "TestScene")
{
}

//初期化
void TestScene::Initialize()
{	
	// 真上から見下ろすカメラ
	float centerX = 15.0f * 15.0f;
	float centerZ = 15.0f * 15.0f;
	float height = 16.0f * 30.0f;

//	Instantiate <Player>(this);
	Instantiate <DungeonManager>(this);
	Camera::SetTarget(XMFLOAT3(15.0f * 15.0f, 0.0f, 15.0f * 15.0f));
}

//更新
void TestScene::Update()
{
	// キーで上下前後左右移動したい
	if (Input::IsKey(DIK_UP)) {
		Camera::SetPosition(Camera::GetPosition().x, Camera::GetPosition().y - 1.0f, Camera::GetPosition().z);
	}
	if (Input::IsKey(DIK_DOWN)) {
		Camera::SetPosition(Camera::GetPosition().x, Camera::GetPosition().y + 1.0f, Camera::GetPosition().z);
	}
	if (Input::IsKey(DIK_LEFT)) {
		Camera::SetPosition(Camera::GetPosition().x - 1.0f, Camera::GetPosition().y, Camera::GetPosition().z);
	}
	if (Input::IsKey(DIK_RIGHT)) {
		Camera::SetPosition(Camera::GetPosition().x + 1.0f, Camera::GetPosition().y, Camera::GetPosition().z);
	}
	if (Input::IsKey(DIK_W)) {
		Camera::SetPosition(Camera::GetPosition().x, Camera::GetPosition().y, Camera::GetPosition().z + 1.0f);
	}
	if (Input::IsKey(DIK_S)) {
		Camera::SetPosition(Camera::GetPosition().x, Camera::GetPosition().y, Camera::GetPosition().z - 1.0f);
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
