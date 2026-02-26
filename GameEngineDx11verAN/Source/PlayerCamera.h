#pragma once
#include <DirectXMath.h>

class PlayerCamera
{
public:

	/// <summary>
	/// プレイヤーカメラの初期化
	/// </summary>
	/// <param name="_yawDeg"></param>
	/// <param name="_pitchDeg"></param>
	/// <param name="_distance"></param>
	void Initialize(float _yawDeg, float _pitchDeg, float _distance);

	/// <summary>
	/// プレイヤーカメラの更新
	/// </summary>
	/// <param name="_targetPos"></param>
	void Update(const DirectX::XMFLOAT3& _targetPos);

	DirectX::XMFLOAT3 GetFocus() const { return focus_; }
	DirectX::XMFLOAT3 GetCameraPosition() const { return camPos_; }

private:
	// 調整用
	float mouseSens_;
	float zoomSens_;

	// 上下限指定
	float minPitchDeg_;
	float maxPitchDeg_;
	float minDistance_;
	float maxDistance_;

	float yawRad_;
	float pitchRad_;
	float distance_;

	// プレイヤーからカメラへのベクトル
	DirectX::XMFLOAT3 focus_;
	
	DirectX::XMFLOAT3 camPos_;

	
};