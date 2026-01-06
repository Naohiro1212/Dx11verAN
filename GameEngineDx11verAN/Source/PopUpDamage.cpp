#include "PopUpDamage.h"
#include "../Engine/Model.h"
#include "../Engine/GameTime.h"

namespace
{
	const float DISPLAY_DURATION = 3.0f; // ダメージ表示時間
	const float DIGIT_OFFSET_X = 5.0f;   // 桁間隔のX方向オフセット
}

PopUpDamage::PopUpDamage(GameObject* parent)
{
}

void PopUpDamage::Initialize()
{
	LoadDigitModels();
	displayTime_ = 0.0f;
	transform_.scale_ = XMFLOAT3(0.1f, 0.1f, 0.1f);
}

void PopUpDamage::Update()
{
	float dt_ = GameTime::DeltaTime();
	displayTime_ += dt_;

	// 少しづつ上に移動（2桁以上のダメージ表示を考慮して中央揃えにする）
	transform_.position_.y += 15.0f * dt_;

	// 一定時間経過したら消す
	if (displayTime_ >= DISPLAY_DURATION)
	{
		KillMe();
		return;
	}
}

void PopUpDamage::Draw()
{
	// transform_ は Update で現在位置が設定されているので、それを元に桁描画する。
	int firstDigit_ = damage_ / 10;
	int secondDigit_ = damage_ % 10;
	DirectX::XMFLOAT3 basePosition = transform_.position_;
	// 頭上に表示するため、Y座標を少し上げる

	// 元の transform_ を直接書き換えないよう、一時 Transform を使って描画する
	Transform tempTransform = transform_; // Transform 型は GameObject から利用可能

	if (firstDigit_ > 0)
	{
		// 2桁表示：左（十の位）、右（一の位）
		DirectX::XMFLOAT3 firstDigitPos = XMFLOAT3(basePosition.x, basePosition.y + 20.0f, basePosition.z);
		firstDigitPos.x -= DIGIT_OFFSET_X;
		tempTransform.position_ = firstDigitPos;
		Model::SetTransform(digitModels_[firstDigit_], tempTransform);
		Model::Draw(digitModels_[firstDigit_]);

		DirectX::XMFLOAT3 secondDigitPos = XMFLOAT3(basePosition.x, basePosition.y + 20.0f, basePosition.z);
		secondDigitPos.x += DIGIT_OFFSET_X;
		tempTransform.position_ = secondDigitPos;
		Model::SetTransform(digitModels_[secondDigit_], tempTransform);
		Model::Draw(digitModels_[secondDigit_]);
	}
	else
	{
		// 1桁表示：中央
		tempTransform.position_ = basePosition;
		Model::SetTransform(digitModels_[secondDigit_], tempTransform);
		Model::Draw(digitModels_[secondDigit_]);
	}
}

void PopUpDamage::Release()
{
}

void PopUpDamage::PreLoadDigitModels()
{
	if (!digitModels_.empty())
	{
		return;
	}
	for (int i = 0; i <= 9; ++i)
	{
		std::string modelPath = "digit" + std::to_string(i) + ".fbx";
		int modelHandle = Model::Load(modelPath.c_str());
		digitModels_.push_back(modelHandle);
	}
}

// 既存の LoadDigitModels はそのままでも構いません（互換性のために残す）
void PopUpDamage::LoadDigitModels()
{
	PreLoadDigitModels();
}