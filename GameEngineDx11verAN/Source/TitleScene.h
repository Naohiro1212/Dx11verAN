#pragma once
#include "../Engine/GameObject.h"

class Button;
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

	// 仮ボタン
	Button* startButton_;
	Button* endButton_;

	// 背景
	int titleImage_;
	Transform bgTransform_;

	// BGM
	int bgmHandle_;
};