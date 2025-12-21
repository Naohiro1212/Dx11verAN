#pragma once
#include "../Engine/GameObject.h"

class Button : public GameObject
{
public:
	// コンストラクタ
	// 引数：parent  親オブジェクト
	Button(GameObject* parent);
	// 初期化
	void Initialize() override;

	// 更新
	void Update() override;
	// 描画
	void Draw() override;
	// 開放
	void Release() override;

	// セッターゲッター
	void SetButtonImage(int imageHandle) { buttonImage_ = imageHandle; }
	void SetButtonScale(float width, float height);
	void SetButtonName(const std::string& name) { objectName_ = name; }
	void SetButtonPosition(float x, float y) { transform_.position_.x = x; transform_.position_.y = y; }
	bool GetOnButton() { return onButton_; }
	// 中央基準にするかどうか
	void SetCenter(bool center) { center_ = center; }

	// 大きさをリセット
	void ResetButtonSize();

private:
	bool onButton_;
	int buttonImage_;

	// 描画基準が中央かどうか
	bool center_;
};