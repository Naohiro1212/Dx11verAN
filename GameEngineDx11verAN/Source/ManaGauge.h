#pragma once
#include "../Engine/GameObject.h"

class ManaGauge : public GameObject
{
public:
	// コンストラクタ
	// 引数：parent  親オブジェクト（Player）
	ManaGauge(GameObject* parent);
	// 初期化
	void Initialize() override;
	// 更新
	void Update() override;
	// 描画
	void Draw() override;
	// 開放
	void Release() override;

	void SetMana(float mana) { mana_ = mana; }
	void SetMaxMana(float maxMana) { maxMana_ = maxMana; }

private:
	float mana_;
	float maxMana_;
};