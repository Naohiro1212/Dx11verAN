#pragma once
#include "../Engine/GameObject.h"
#include "PlayerCamera.h"

class BoxCollider;

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

	void OnCollision(GameObject* pTarget) override;

private:
	// モデルハンドル
	int walkModel_;
	int runModel_;
	int leftStrafeModel_;
	int rightStrafeModel_;
	int backStrafeModel_;
	int idleModel_;
	int slashModel_;
	int nowModel_;

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

	// 攻撃時間計測
	float attackTimer_;

	// ジャンプ上限
	size_t jumpCount_;

	bool onGround_;
	bool isAttacking_;
	bool prevMouseLeftDown_;
	float lastSlashFrame_;

	// 当たり判定
	BoxCollider* pCollider_;
	BoxCollider* attackCollider_;
	// 攻撃用コライダーの回転を反映した中心オフセット
	XMFLOAT3 rotateCenter_;

	// 魔法攻撃用方向ベクトル
	XMFLOAT3 magicDir_;
};