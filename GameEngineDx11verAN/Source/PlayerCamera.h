#pragma once
#include "../Engine/GameObject.h"
#include <DirectXMath.h>
#include <vector>

class BoxCollider;

class PlayerCamera : public GameObject
{
public:

	//コンストラクタ
	PlayerCamera(GameObject* parent);
	//デストラクタ
	~PlayerCamera();

	/// <summary>
	/// 引数なしのInitialize
	/// </summary>
	void Initialize() override {}
	void Update() override {}
	void Draw() override {}
	void Release() override {}

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
	void Update(const DirectX::XMFLOAT3& _targetPos, std::vector<BoxCollider*> colliders);

	DirectX::XMFLOAT3 GetFocus() const { return focus_; }

	/// <summary>
	/// カメラの壁ずり処理 
	/// </summary>
	void ResolveWallCollisions();

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

	// 壁との当たり判定用
	BoxCollider* pCollider_;
	std::vector<BoxCollider*> wallColliders_;
};