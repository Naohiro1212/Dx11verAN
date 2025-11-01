#pragma once
#include "../Engine/GameObject.h"
#include "PlayerCamera.h"

class Player :
    public GameObject
{
public:
	//コンストラクタ
	//引数：parent  親オブジェクト（SceneManager）
	Player(GameObject* parent);

	//初期化
	void Initialize() override;

	//更新
	void Update() override;

	//描画
	void Draw() override;

	//開放
	void Release() override;
	
	//カメラ移動
	void UpdateOrbitCamera(const XMFLOAT3& _targetPos);


private:
	int hSilly;

	// カメラパラメーター
	float camYawRad_;
	float camPitchRad_;
	float camDistance_;

	float minPitchRad_;
	float maxPitchRad_;
	float minDistance_;
	float maxDistance_;

	PlayerCamera plvision_;

	bool wasMoving_;
	// ジャンプ関連
	float JumpV0_;
	float velocityY_;

	// ジャンプ上限
	size_t jumpCount_;

	bool onGround_;
};

