#pragma once
#include "../Engine/GameObject.h"

class Button;
class Text;
class Sprite;

// タイトルシーンを管理するクラス
class TitleScene : public GameObject
{
public:
	// コンストラクタ
	// 引数：parent  親オブジェクト（SceneManager）
	TitleScene(GameObject* parent);
	// 初期化
	void Initialize() override;
	// 更新
	void Update() override;
	// 描画
	void Draw() override;
	// 開放
	void Release() override;

private:

	// テキスト
	Text* pText_;

	// 仮ボタン
	Button* pButton_;

	// 背景
	int titleImage_;
	Transform bgTransform_;
};