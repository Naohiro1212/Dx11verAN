#pragma once
#include "../Engine/GameObject.h"

// カーソル管理クラス
class CursorManager : public GameObject
{
public:
	// コンストラクタ
	// 引数：parent  親オブジェクト（SceneManager）
	CursorManager(GameObject* parent);
	// 初期化
	void Initialize() override;
	// 更新
	void Update() override;
	// 描画
	void Draw() override;
	// 開放
	void Release() override;

private:
	// カーソル画像ハンドル
	int cursorImageHandle_;
};