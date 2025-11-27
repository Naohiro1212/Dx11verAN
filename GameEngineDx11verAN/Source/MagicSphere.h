#pragma once
#include "../Engine/GameObject.h"

class MagicSphere : public GameObject
{
public:
	//コンストラクタ
	MagicSphere(GameObject* parent);
	//デストラクタ
	~MagicSphere();
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
	//描画
	void Draw() override;
	//開放
	void Release() override;

	void SetMoveVec(XMFLOAT3 moveVec) { moveVec_ = moveVec; }

private:
	int magicModel_;
	float AttackTimer_;
	XMFLOAT3 moveVec_;
};