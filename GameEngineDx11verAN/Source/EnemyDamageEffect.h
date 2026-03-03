#pragma once
#include "../Engine/GameObject.h"
#include "../Engine/VFX.h"

enum EffectType
{
	BLOOD_EFFECT = 0,
	SLASH_EFFECT
};

class EnemyDamageEffect : public GameObject
{
public:
	//コンストラクタ
	//引数：parent  親オブジェクト（SceneManager）
	EnemyDamageEffect(GameObject* parent, XMFLOAT3 pos);
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
	//描画
	void Draw() override;
	//開放
	void Release() override;

private:
	// 経過時間計測用
	float timer_;
	float slashTimer_;

	// VFXハンドル
	// 流血エフェクト
	EmitterData data_;

	// 斬撃エフェクト
	EmitterData slashData_;
	int hEmit_[2];
};