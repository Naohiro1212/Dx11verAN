#pragma once
#include "../Engine/GameObject.h"
#include "../Engine/VFX.h"

class LevelUpEffect : public GameObject
{
public:
	//コンストラクタ
	//引数：parent  親オブジェクト（SceneManager）
	LevelUpEffect(GameObject* parent, XMFLOAT3 pos);
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

	// VFXハンドル
	EmitterData data_;
	int hEmit_;
};