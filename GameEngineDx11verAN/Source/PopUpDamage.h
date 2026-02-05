#pragma once
#include "../Engine/GameObject.h"
#include <vector>

class BillBoard;

enum class DamageType
{
	FromEnemy,
	ToEnemy
};

class PopUpDamage : public GameObject
{
public:
	//コンストラクタ
	//引数：parent  親オブジェクト（SceneManager）
	PopUpDamage(GameObject* parent);
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
	//描画
	void Draw() override;
	//開放
	void Release() override;

	void SetDamage(int damage) { damage_ = damage; }

	void SetDamageType(DamageType type) { damageType_ = type; }

	DamageType damageType_;
private:
	BillBoard* digitBillboards_[10]; // 0～9
	int damage_;

	// 表示する時間
	float displayTime_;

	// 敵からダメージを受けたか、与えたかのフラグ
	XMFLOAT4 color_;
};