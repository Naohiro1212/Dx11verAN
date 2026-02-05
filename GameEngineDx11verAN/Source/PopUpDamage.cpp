#include "PopUpDamage.h"
#include "../Engine/Image.h"
#include "../Engine/GameTime.h"
#include "../Engine/Billboard.h"
#include "../Engine/Camera.h"

namespace
{
	const float DISPLAY_DURATION = 3.0f; // ダメージ表示時間
	const float DIGIT_OFFSET_X = 2.5f;   // 桁間隔のX方向オフセット
}

PopUpDamage::PopUpDamage(GameObject* parent) : GameObject(parent, "PopUpDamage")
{
}

void PopUpDamage::Initialize()
{
	for (int i = 0; i < 10; ++i)
	{
		digitBillboards_[i] = new BillBoard();
		std::string fileName = "digit" + std::to_string(i) + ".png";
		digitBillboards_[i]->Load(fileName);
	}

	displayTime_ = 0.0f;
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
	if (damageType_ == DamageType::FromEnemy)
	{
		color_ = XMFLOAT4(1.0f, 0.2f, 0.2f, 1.0f); // 赤系
	}
	else // DamageType::ToEnemy
	{
		color_ = XMFLOAT4(0.2f, 0.6f, 1.0f, 1.0f); // 青系
	}

    Direct3D::SetShader(Direct3D::SHADER_BILLBOARD);

    int firstDigit = damage_ / 10;
    int secondDigit = damage_ % 10;

    XMFLOAT3 basePos = transform_.position_;
    basePos.y += 20.0f;  // 頭上オフセット

    XMMATRIX matScale_ = XMMatrixScaling(2.5f, 2.5f, 1.0f);
    XMMATRIX matBill_ = Camera::GetBillboardMatrix();

    if (firstDigit > 0)
    {
        // 十の位：ローカルで左へ
        {
            float localX = -DIGIT_OFFSET_X;   // ローカル左
            XMMATRIX matLocalTrans_ = XMMatrixTranslation(localX, 0.0f, 0.0f);

            XMMATRIX matWorld_ =
                matScale_
                * matLocalTrans_                // ローカルで「左」にずらす
                * matBill_                      // カメラを向く回転
                * XMMatrixTranslation(basePos.x, basePos.y, basePos.z);

            digitBillboards_[firstDigit]->Draw(matWorld_, color_);
        }

        // 一の位：ローカルで右へ
        {
            float localX = DIGIT_OFFSET_X;   // ローカル右
            XMMATRIX matLocalTrans_ = XMMatrixTranslation(localX, 0.0f, 0.0f);

            XMMATRIX matWorld_ =
                matScale_
                * matLocalTrans_
                * matBill_
                * XMMatrixTranslation(basePos.x, basePos.y, basePos.z);

            digitBillboards_[secondDigit]->Draw(matWorld_, color_);
        }
    }
    else
    {
        // 1桁：中央（ローカルオフセットなし）
        XMMATRIX matWorld_ =
            matScale_
            * matBill_
            * XMMatrixTranslation(basePos.x, basePos.y, basePos.z);

        digitBillboards_[secondDigit]->Draw(matWorld_, color_);
    }

    Direct3D::SetShader(Direct3D::SHADER_3D);
    Direct3D::SetBlendMode(Direct3D::BLEND_DEFAULT);
}

void PopUpDamage::Release()
{
}