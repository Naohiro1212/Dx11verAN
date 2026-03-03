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
	pCollider_(nullptr)
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
        XMFLOAT3(3.0f, 3.0f, 3.0f)  // サイズ（適当に小さめの立方体）
	);
    AddCollider(pCollider_);
    pCollider_->SetRole(Collider::Role::Body);
}

void PlayerCamera::Update(const XMFLOAT3& _targetPos)
{
	float dt_ = GameTime::DeltaTime();

    //Debug::Log(transform_.position_.x, false);
    //Debug::Log(transform_.position_.y, false);
    //Debug::Log(transform_.position_.z, true);

    XMFLOAT3 md_ = Input::GetMouseMove();
    float dx_ = md_.x;
    float dy_ = md_.y;
    float wheelSteps_ = md_.z / 120.0f;

    // rad変換する
	float minPitchRad_ = XMConvertToRadians(minPitchDeg_);
	float maxPitchRad_ = XMConvertToRadians(maxPitchDeg_);

    // 角度更新
    yawRad_ += dx_ * mouseSens_;
    pitchRad_ = std::clamp(pitchRad_ + dy_ * mouseSens_, minPitchRad_, maxPitchRad_);

    // ズーム更新（dt でスムーズに）
    if (wheelSteps_ != 0.0f)
    {
        distance_ = std::clamp(distance_ - wheelSteps_ * zoomSens_, minDistance_, maxDistance_);
    }

	focus_ = { _targetPos.x, _targetPos.y + CAMERA_DISTANCE, _targetPos.z};

    float radius_ = std::clamp(distance_ + 6.0f,
        minDistance_, maxDistance_ + 6.0f);

	float cp_ = std::cos(pitchRad_);
	float sp_ = std::sin(pitchRad_);
	float cy_ = std::cos(yawRad_);
	float sy_ = std::sin(yawRad_);

	float offX = radius_ * cp_ * sy_;
	float offY = radius_ * sp_;
    float offZ = radius_ * cp_ * cy_;

	transform_.position_ = { focus_.x + offX, focus_.y + offY, focus_.z + offZ };

    // カメラは必ず地上
    // 上限と下限を設定してその間にする
	transform_.position_.y = (std::clamp)(transform_.position_.y, focus_.y + MIN_CAMERA_HEIGHT, focus_.y + MAX_CAMERA_HEIGHT); 
    // 壁を考慮したカメラ位置補正

    Camera::SetTarget({ focus_.x, focus_.y, focus_.z });
	Camera::SetPosition(transform_.position_.x, transform_.position_.y, transform_.position_.z);
}

void PlayerCamera::ResolveWallCollisions(BoxCollider* _wallBox)
{
    Debug::Log("CamBox pos:", false);
    Debug::Log(transform_.position_.x, false);
    Debug::Log(transform_.position_.y, false);
    Debug::Log(transform_.position_.z, true);

    PenetrationResult res = Collider::ComputeBoxVsBoxPenetration(pCollider_, _wallBox);
    if (res.overlapped)
    {
        Debug::Log("deteimasu");
    }
}

void PlayerCamera::OnCollision(GameObject* pTarget)
{
    Debug::Log("PlayerCamera::OnCollision CALLED, target=" + pTarget->GetObjectName(), true);
}