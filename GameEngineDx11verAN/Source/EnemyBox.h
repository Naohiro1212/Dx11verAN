#pragma once
#include "../Engine/GameObject.h"

class EnemyBox :
	public GameObject
{
	int hSilly;
public:
	//コンストラクタ
	//引数：parent  親オブジェクト（SceneManager）
	EnemyBox(GameObject* parent);

	//初期化
	void Initialize() override;

	//更新
	void Update() override;

	//描画
	void Draw() override;

	//開放
	void Release() override;
};
