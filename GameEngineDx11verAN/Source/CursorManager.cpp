#include "CursorManager.h"
#include "../Engine/Image.h"
#include "../Engine/Input.h"

CursorManager::CursorManager(GameObject* parent) : GameObject(parent, "CursorManager"), cursorImageHandle_(-1)
{
}

void CursorManager::Initialize()
{
	// カーソル画像読み込み
	cursorImageHandle_ = Image::Load("cursor.png");
	SetDrawOrder(0); // 最前面に描画
	SetIsUIObject(true); // UIオブジェクトに設定
}

void CursorManager::Update()
{
	XMFLOAT3 mousePos_ = Input::GetMousePosition();
	Image::SetPositionPixels(cursorImageHandle_, mousePos_.x, mousePos_.y, false);
}

void CursorManager::Draw()
{
	Image::Draw(cursorImageHandle_);
}

void CursorManager::Release()
{
}
