#include "TestScene.h"
#include "Player.h"

TestScene::TestScene(GameObject* parent) : GameObject(parent, "TestScene")
{
}

TestScene::~TestScene()
{
}

// ‰Šú‰»
void TestScene::Initialize()
{
	Instantiate <Player>(this);
}

void TestScene::Update()
{
}

void TestScene::Draw()
{
}

void TestScene::Release()
{
}
