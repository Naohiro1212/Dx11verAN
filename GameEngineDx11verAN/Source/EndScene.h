#pragma once
#include "../Engine/GameObject.h"

class Button;
class Text;

class EndScene : public GameObject
{
public:
	// コンストラクタ
	// 引数：parent  親オブジェクト（SceneManager）
	EndScene(GameObject* parent);
	// 初期化
	void Initialize() override;
	// 更新
	void Update() override;
	// 描画
	void Draw() override;
	// 開放
	void Release() override;

private:
	int EndImage_;

	// クリックしたらタイトルに戻るボタン
	Button* pButton_;

	Transform bgTransform_;

	// スコアテキスト
	Text* pScoreText_;
};