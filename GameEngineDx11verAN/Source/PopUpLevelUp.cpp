#include "PopUpLevelUp.h"
#include "../Engine/GameTime.h"
#include "../Engine/BillBoard.h"
#include "../Engine/Camera.h"

namespace
{
	const float DISPLAY_DURATION = 1.8f; // レベルアップ表示時間
	const float HEAD_OFFSET_Y = 20.0f;        // 頭上表示のYオフセット

	const XMFLOAT3 SCALE = { 3.0f, 1.0f, 1.0f };
	const XMFLOAT4 COLOR = { 1.0f, 1.0f, 1.0f, 1.0f };
}

PopUpLevelUp::PopUpLevelUp(GameObject* parent) : GameObject(parent, "PopUpLevelUp")
{
}

void PopUpLevelUp::Initialize()
{
	levelUpBillboard_ = new BillBoard();
	levelUpBillboard_->Load("levelUpPoptest.png");

	displayTime_ = 0.0f;
	color_ = COLOR;
}

void PopUpLevelUp::Update()
{
	float dt_ = GameTime::DeltaTime();
	displayTime_ += dt_;

	// 一定時間経過したら消す
	if (displayTime_ >= DISPLAY_DURATION)
	{
		KillMe();
		return;
	}
}

void PopUpLevelUp::Draw()
{
	// 頭上に表示する
	// Billboard描画出来るようにシェーダー切り替え
	Direct3D::SetShader(Direct3D::SHADER_BILLBOARD);
	XMFLOAT3 basePos = transform_.position_;
	basePos.y += HEAD_OFFSET_Y;// 頭上オフセット

	XMMATRIX matScale_ = XMMatrixScaling(SCALE.x, SCALE.y, SCALE.z);
	XMMATRIX matBill_ = Camera::GetBillboardMatrix();

	XMMATRIX T = XMMatrixTranslation(basePos.x, basePos.y, basePos.z);
	XMMATRIX world = matScale_ * matBill_ * T;
	levelUpBillboard_->Draw(world, color_);

	// シェーダーを3Dに戻す
	Direct3D::SetShader(Direct3D::SHADER_3D);
}

void PopUpLevelUp::Release()
{
}
