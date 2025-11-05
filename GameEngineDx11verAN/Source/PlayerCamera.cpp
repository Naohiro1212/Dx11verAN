#include "PlayerCamera.h"
#include "../Engine/Input.h"
#include "../Engine/Camera.h"
#include "../Engine/GameTime.h"
#include <algorithm>
#include <cmath>

using namespace DirectX;

void PlayerCamera::Initialize(float _yawDeg, float _pitchDeg, float _distance) 
{ 
    mouseSens_ = 0.2f;
    zoomSens_ = 2.0f;
    minPitchDeg_ = -50.0f;
    maxPitchDeg_ = 60.0f;
    minDistance_ = 2.0f;
    maxDistance_ = 50.0f;

    // 角度・距離
    yawRad_ = XMConvertToRadians(_yawDeg);
    pitchRad_ = XMConvertToRadians(_pitchDeg);
    distance_ = std::clamp(_distance, minDistance_, maxDistance_);
}

void PlayerCamera::Update(const XMFLOAT3& _targetPos)
{
    XMFLOAT3 md_ = Input::GetMouseMove();
    float dx_ = md_.x;
    float dy_ = md_.y;
    float wheelSteps_ = md_.z / 120.0f;

    // rad変換する
    float minPitchRad_ = XMConvertToRadians(minPitchDeg_);
    float maxPitchRad_ = XMConvertToRadians(maxPitchDeg_);

    // 角度更新
    yawRad_ += dx_ * mouseSens_ * GameTime::DeltaTime();
	pitchRad_ = std::clamp(pitchRad_ + dy_ * mouseSens_ * GameTime::DeltaTime(), minPitchRad_, maxPitchRad_);

    if (wheelSteps_ != 0.0f)
    {
		distance_ = std::clamp(distance_ - wheelSteps_ * zoomSens_, minDistance_, maxDistance_);
    }

    focus_ = _targetPos;

	float cp_ = std::cos(pitchRad_);
	float sp_ = std::sin(pitchRad_);
	float cy_ = std::cos(yawRad_);
	float sy_ = std::sin(yawRad_);

	float offX = distance_ * cp_ * sy_;
	float offY = distance_ * sp_;
    float offZ = distance_ * cp_ * cy_;

	camPos_ = { focus_.x + offX,
				 focus_.y + offY,
				 focus_.z + offZ };

    Camera::SetTarget(focus_);
	Camera::SetPosition(camPos_.x, camPos_.y, camPos_.z);
}

