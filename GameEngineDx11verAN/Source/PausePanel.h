#pragma once
#include "../Engine/GameObject.h"

class PausePanel : public GameObject
{
public:
	// コンストラクタ
	// 引数：parent  親オブジェクト（TestScene）
	PausePanel(GameObject* parent);
	// 初期化
	void Initialize() override;
	// 更新
	void Update() override;
	// 描画
	void Draw() override;
	// 開放
	void Release() override;

	// ポーズ状態の設定
	void SetPaused(bool paused) { nowPaused_ = paused; };

private:
	int panelImage_;
	bool nowPaused_;
};