#include "SkillPanel.h"
#include "../Engine/Button.h"
#include "../Engine/Text.h"
#include "../Source/Player.h"
#include "../Engine/Direct3D.h"
#include "../Engine/Input.h"
#include <string>

namespace
{
	// 現段階でのスキルの最大枚数
	const float MAX_PANEL_NUM = 2;
	const float HALF_WINDOW_WIDTH = 0.5f;
	const float PANEL_SPACING = 150.0f;
}

SkillPanel::SkillPanel(GameObject* parent) : GameObject(parent)
{
}

void SkillPanel::Initialize()
{
	LoadPanelInfo();

	SetDrawOrder(2);
	SetIsUIObject(true);

	for (int i = 0;i < MAX_PANEL_NUM; ++i)
	{
		skillSelectPanels_[i].button_->Invisible();
	}
}

void SkillPanel::Update()
{
	XMFLOAT3 mousePos = Input::GetMousePosition();

	for (int i = 0;i < MAX_PANEL_NUM; ++i)
	{
		skillSelectPanels_[i].button_->Update();
		skillSelectPanels_[i].onButton_ = skillSelectPanels_[i].button_->GetOnButton();
		
		// 画面中央を基準に左右に配置
		float centerX = Direct3D::screenWidth_ * HALF_WINDOW_WIDTH;
		float centerY = Direct3D::screenHeight_ * HALF_WINDOW_WIDTH;
		float offsetX = (i - (MAX_PANEL_NUM - 1) / 2.0f) * PANEL_SPACING; // 左右に等間隔で配置

		skillSelectPanels_[i].button_->SetButtonPosition(centerX + offsetX, centerY);
	}

	if (Input::IsMouseButtonDown(0))
	{
		for (int i = 0;i < MAX_PANEL_NUM; ++i)
		{
			if (skillSelectPanels_[i].onButton_)
			{
				// スキル選択の処理をここに書く
				// 例: プレイヤーのスキルレベルを上げる、スキルの効果を適用するなど

			}
		}
	}
}

void SkillPanel::Draw()
{
}

void SkillPanel::Release()
{
}

void SkillPanel::LoadPanelInfo()
{
	for (int i = 0;i < MAX_PANEL_NUM; ++i)
	{
		SkillSelectPanelInfo info;
		info.button_ = Instantiate<Button>(this);
		info.button_->SetCenter(true);
		info.button_->SetButtonImage(Image::Load("SkillPanel" + std::to_string(i) + ".png"));
		
		// 画面中央を基準に左右に配置
		float centerX = Direct3D::screenWidth_ * HALF_WINDOW_WIDTH;
		float centerY = Direct3D::screenHeight_ * HALF_WINDOW_WIDTH;
		float offsetX = (i - (MAX_PANEL_NUM - 1) / 2.0f) * PANEL_SPACING; // 左右に等間隔で配置

		info.button_->SetButtonPosition(centerX + offsetX, centerY);

		info.onButton_ = false;
		skillSelectPanels_.push_back(info);
	}
}
