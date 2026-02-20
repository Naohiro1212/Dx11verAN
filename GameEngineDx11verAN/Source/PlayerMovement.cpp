#include "PlayerMovement.h"
#include "../Source/Plane.h"
#include "../Engine/GameObject.h"
#include "../Engine/GameTime.h"
#include "../Engine/Input.h"

PlayerMovement::PlayerMovement(PlayerConfig cnf, Transform& transform, Plane* pPlane)
	: cnf_(cnf), transform_(transform), pPlane_(pPlane)
{

}

PlayerMovement::~PlayerMovement()
{
}

void PlayerMovement::Initialize()
{
	transform_.position_ = { 0.0f, 0.0f, 0.0f };
	transform_.rotate_ = { 0.0f, 0.0f, 0.0f };

	JumpV0_ = sqrtf(cnf_.JUMP_V0_CONSTANT);
	velocityY_ = 0.0f;

	jumpCount_ = 0;
	onGround_ = true;
}

void PlayerMovement::Update()
{
	dt_ = GameTime::DeltaTime();


}

void PlayerMovement::ResolveWallCollisions(std::vector<BoxCollider*> wallColliders)
{
}

void PlayerMovement::UpdateYawToCamera(const XMFLOAT3& cameraForward, float& playerYaw)
{
    float targetYawRad = atan2f(cameraForward.x, cameraForward.z);
    float targetYawDeg = XMConvertToDegrees(targetYawRad) + cnf_.FACE_OFFSET_DEG;
    float diff = targetYawDeg - playerYaw;
    while (diff > cnf_.HALF_TURN) diff -= cnf_.FULL_TURN;
    while (diff < -cnf_.HALF_TURN) diff += cnf_.FULL_TURN;
    float step = cnf_.TURN_SPEED_DEG * dt_;
    if (fabsf(diff) <= step)
    {
        playerYaw = targetYawDeg;
    }
    else 
    {
        playerYaw += (diff > 0 ? step : -step);
    }
}

void PlayerMovement::MoveInput(bool isAttacking, float health)
{
    // 入力を +1/0/-1 に畳む（カメラ相対移動: W/S=前後, A/D=ストレイフ）
	moveDir_ = { 0, 0 };

    // 攻撃しておらず、なおかつ死んでいない状態でのみ移動入力を受け付ける
    if (!isAttacking && health > 0.0f)
    {
        if (Input::IsKey(DIK_W)) {
            moveDir_.fwd_ += 1;
        }
        if (Input::IsKey(DIK_S)) {
            moveDir_.fwd_ -= 1;
        }
        if (Input::IsKey(DIK_D)) {
            moveDir_.str_ += 1;
        }
        if (Input::IsKey(DIK_A)) {
            moveDir_.str_ -= 1;
        }
    }

    isMovingNow_ = false;
    if (moveDir_.fwd_ != 0 || moveDir_.str_ != 0) {
        isMovingNow_ = true;
    }
}

void PlayerMovement::Jump()
{
}

void PlayerMovement::Dash()
{
}

void PlayerMovement::UpdateGravity()
{
}

XMFLOAT3 PlayerMovement::SlideAlongWall(const XMFLOAT3& f, const XMFLOAT3& n)
{
    return XMFLOAT3();
}
