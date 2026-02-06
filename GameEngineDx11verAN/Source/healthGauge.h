#pragma once
#include "../Engine/GameObject.h"

class HealthGauge : public GameObject
{
public:
	// コンストラクタ
	// 引数：parent  親オブジェクト（Player）
	HealthGauge(GameObject* parent);
	// 初期化
	void Initialize() override;
	// 更新
	void Update() override;
	// 描画
	void Draw() override;
	// 開放
	void Release() override;
	void SetHealth(float health) { health_ = health; }
	void SetMaxHealth(float maxHealth) { maxHealth_ = maxHealth; }

private:
	float health_;
	float maxHealth_;

	// 現在の体力量に応じた割合
	float ratio_;

	// ゲージ画像ハンドル
	int gaugeImage_;
	int frameImage_;
	float posX_;
	float posY_;
};