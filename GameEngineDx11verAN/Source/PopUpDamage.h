#pragma once
#include "../Engine/GameObject.h"
#include <vector>

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

	void LoadDigitModels();

	static void PreLoadDigitModels();

private:
	inline static std::vector<int> digitModels_;
	int damage_;

	// 表示する時間
	float displayTime_;
};