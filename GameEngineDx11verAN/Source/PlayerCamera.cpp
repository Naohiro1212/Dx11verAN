#include "PlayerCamera.h"
#include "../Engine/Input.h"
#include "../Engine/Camera.h"
#include "../Engine/GameTime.h"
#include <algorithm>
#include <cmath>
#include <vector>
#include "../Engine/BoxCollider.h"
#include "../Engine/Debug.h"

using namespace DirectX;

namespace
{
    const float CAMERA_DISTANCE = 15.0f;
	const float MIN_CAMERA_HEIGHT = 1.8f;
    const float MAX_CAMERA_HEIGHT = 45.0f;
}

PlayerCamera::PlayerCamera(GameObject* parent) : GameObject(parent, "PlayerCamera"),
    mouseSens_(0.004f),
    zoomSens_(0.5f),
    minPitchDeg_(-30.0f),
    maxPitchDeg_(40.0f),
    minDistance_(5.0f),
    maxDistance_(25.0f),
    yawRad_(0.0f),
    pitchRad_(0.0f),
    distance_(CAMERA_DISTANCE),
    focus_({ 0.0f, 0.0f, 0.0f }),
	pCollider_(nullptr),
	collisionPush_({ 0.0f, 0.0f, 0.0f })
{
}

PlayerCamera::~PlayerCamera()
{
}

void PlayerCamera::Initialize(float _yawDeg, float _pitchDeg, float _distance)
{ 
    mouseSens_ = 0.004f;
    zoomSens_ = 2.3f;
    minPitchDeg_ = -30.0f;
    maxPitchDeg_ = 40.0f;
    minDistance_ = 20.0f;
    maxDistance_ = 60.0f;

    // 角度・距離
    yawRad_ = XMConvertToRadians(_yawDeg);
    pitchRad_ = XMConvertToRadians(_pitchDeg);
    distance_ = std::clamp(_distance, minDistance_, maxDistance_);

	transform_.position_ = { 0.0f, 0.0f, 0.0f };

    pCollider_ = new BoxCollider(
        XMFLOAT3(0.0f, 0.0f, 0.0f), // カメラ位置からの相対位置（カメラの中心点）
        XMFLOAT3(0.5f, 0.5f, 0.5f)  // サイズ（適当に小さめの立方体）
	);
    AddCollider(pCollider_);
    pCollider_->SetRole(Collider::Role::Body);
}

void PlayerCamera::Update(const XMFLOAT3& _targetPos)
{
    float dt_ = GameTime::DeltaTime();

    XMFLOAT3 md_ = Input::GetMouseMove();
    float dx_ = md_.x;
    float dy_ = md_.y;
    float wheelSteps_ = md_.z / 120.0f;

    float minPitchRad_ = XMConvertToRadians(minPitchDeg_);
    float maxPitchRad_ = XMConvertToRadians(maxPitchDeg_);

    yawRad_ += dx_ * mouseSens_;
    pitchRad_ = std::clamp(pitchRad_ + dy_ * mouseSens_, minPitchRad_, maxPitchRad_);

    if (wheelSteps_ != 0.0f)
    {
        distance_ = std::clamp(distance_ - wheelSteps_ * zoomSens_, minDistance_, maxDistance_);
    }

    focus_ = { _targetPos.x, _targetPos.y + CAMERA_DISTANCE, _targetPos.z };

    float radius_ = std::clamp(distance_ + 6.0f, minDistance_, maxDistance_ + 6.0f);

    float cp = std::cos(pitchRad_);
    float sp = std::sin(pitchRad_);
    float cy = std::cos(yawRad_);
    float sy = std::sin(yawRad_);

    float offX = radius_ * cp * sy;
    float offY = radius_ * sp;
    float offZ = radius_ * cp * cy;

    // 1. 理想位置
    XMFLOAT3 idealPos = { focus_.x + offX, focus_.y + offY, focus_.z + offZ };
    idealPos.y = std::clamp(idealPos.y, focus_.y + MIN_CAMERA_HEIGHT, focus_.y + MAX_CAMERA_HEIGHT);

    transform_.position_ = idealPos;

    // pCollider_ が transform 追従型かどうかでやり方が変わる
    // 追従しないタイプなら、ここで一時的に center を idealPos にして判定する
    // pCollider_->SetCenter(idealPos);

    const float margin = 0.6f;

    for (auto* wall : wallColliders_)
    {
        PenetrationResult res = Collider::ComputeBoxVsBoxPenetration(pCollider_, wall);
        if (res.overlapped)
        {
            // res.push は「idealPos からどれだけ動かせば重なりが解消するか」のベクトル想定
            // 少し margin を足して、完全に外側へ
            XMFLOAT3 dir = res.normal;
            float len = std::sqrt(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
            if (len > 0.0001f)
            {
                dir.x /= len;
                dir.y /= len;
                dir.z /= len;

                res.push.x += dir.x * margin;
                res.push.y += dir.y * margin;
                res.push.z += dir.z * margin;
            }

			transform_.position_.x += res.push.x;
			transform_.position_.y += res.push.y;
			transform_.position_.z += res.push.z;

            // 必要ならここで pCollider_ の center も finalPos に更新
            // pCollider_->SetCenter(finalPos);
            break;
        }
    }

    Camera::SetTarget(focus_);
    Camera::SetPosition(transform_.position_.x, transform_.position_.y, transform_.position_.z);
}
