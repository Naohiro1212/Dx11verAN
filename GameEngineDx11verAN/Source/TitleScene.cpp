#include "TitleScene.h"
#include "../Engine/Input.h"
#include "../Engine/SceneManager.h"

TitleScene::TitleScene(GameObject* parent) : GameObject(parent, "TitleScene")
{
}

void TitleScene::Initialize()
{
}

void TitleScene::Update()
{
	if(Input::IsKeyDown(DIK_SPACE))
	{
		// Enterキーが押されたらシーンを切り替える
		auto sceneManager = dynamic_cast<SceneManager*>(GetParent());
		if (sceneManager)
		{
			sceneManager->ChangeScene(SCENE_ID_TEST);
		}
	}
}

void TitleScene::Draw()
{
}

void TitleScene::Release()
{
}
