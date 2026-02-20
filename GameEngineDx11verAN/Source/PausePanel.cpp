#include "PausePanel.h"
#include "../Engine/Image.h"
#include "../Engine/Direct3D.h"
#include <assert.h>
#include "../Engine/Button.h"
#include "../Engine/Input.h"
#include "../Source/Player.h"
#include "../Engine/Text.h"

namespace
{
	const float HALF_WINDOW_WIDTH = 0.5f;
	const int RESUME_BUTTON_OFFSET_Y = -50;
	const int BACK_TITLE_BUTTON_OFFSET_Y = 120;

	const int LEVEL_TEXT_OFFSET_X = 100;
	const int LEVEL_TEXT_OFFSET_Y = -180;
	const float LEVEL_TEXT_SCALE = 1.7f;
}

PausePanel::PausePanel(GameObject* parent) :
	GameObject(parent),
	panelImage_(-1),
	nowPaused_(false),
	resumeButton_(nullptr),
	BackTitleButton_(nullptr),
	onBTButton_(false),
	onResumeButton_(false),
	player_(nullptr)
{
}

void PausePanel::Initialize()
{
	panelImage_ = Image::Load("PausePanel.png");
	assert(panelImage_ != -1);

	// 読み込み直後に矩形を初期化しておく
	Image::ResetRect(panelImage_);

	// 画像サイズを取得して中央に一度だけ配置する（center=true を使う）
	RECT rect = Image::GetRect(panelImage_);
	float w = (float)(rect.right - rect.left);
	float h = (float)(rect.bottom - rect.top);

	// center=true にすることで SetPositionPixels に x,y を画面中心として渡せる
	Image::SetPositionPixels(panelImage_, Direct3D::screenWidth_ * HALF_WINDOW_WIDTH, 
		Direct3D::screenHeight_ * HALF_WINDOW_WIDTH, true);

	// ボタン初期化
	resumeButton_ = Instantiate<Button>(this);
	resumeButton_->SetCenter(true);
	resumeButton_->SetButtonImage(Image::Load("ResumeButton.png"));
	resumeButton_->SetButtonPosition(Direct3D::screenWidth_ * HALF_WINDOW_WIDTH, 
		Direct3D::screenHeight_ * HALF_WINDOW_WIDTH + RESUME_BUTTON_OFFSET_Y);

	BackTitleButton_ = Instantiate<Button>(this);
	BackTitleButton_->SetCenter(true);
	BackTitleButton_->SetButtonImage(Image::Load("BackTitleButton.png"));
	BackTitleButton_->SetButtonPosition(Direct3D::screenWidth_ * HALF_WINDOW_WIDTH, 
		Direct3D::screenHeight_ * HALF_WINDOW_WIDTH + BACK_TITLE_BUTTON_OFFSET_Y);

	// レベルテキスト初期化
	player_ = dynamic_cast<Player*>(FindObject("Player"));
	levelText_ = new Text();
	levelText_->Initialize();
	levelText_->SetScale(LEVEL_TEXT_SCALE);

	nowPaused_ = false;

	SetDrawOrder(1);
	// UIなのでポーズ時も描画されるようにする
	SetIsUIObject(true);

	// 最初は非表示
	resumeButton_->Invisible();
	BackTitleButton_->Invisible();
}

void PausePanel::Update()
{
	XMFLOAT3 mousePos = Input::GetMousePosition();

	resumeButton_->Update();
	onResumeButton_ = resumeButton_->GetOnButton();
	BackTitleButton_->Update();
	onBTButton_ = BackTitleButton_->GetOnButton();

	// パネルが表示されている状態で、再開ボタンがクリックされたらクリックを消費して再開する
	if (nowPaused_)
	{
		if (onResumeButton_ && Input::IsMouseButtonDown(0))
		{
			nowPaused_ = false;
			// UI がクリックを扱ったので、ゲーム側に同じクリックを渡さない
			Input::ConsumeMouseButtons();
		}
	}
	playerLevel_ = player_->GetLevel();
}

void PausePanel::Draw()
{
	if(nowPaused_)
	{
		Image::Draw(panelImage_);
		resumeButton_->Visible();
		BackTitleButton_->Visible();

		const float centerX = Direct3D::screenWidth_ * HALF_WINDOW_WIDTH;
		const float centerY = Direct3D::screenHeight_ * HALF_WINDOW_WIDTH;

		const int baseX = static_cast<int>(centerX);
		const int baseY = static_cast<int>(centerY);

		// 「LEVEL:」の描画
		levelText_->Draw(baseX - LEVEL_TEXT_OFFSET_X,
			baseY + LEVEL_TEXT_OFFSET_Y,
			"LEVEL:");

		// レベル数値の描画
		levelText_->Draw(baseX + LEVEL_TEXT_OFFSET_X,
			baseY + LEVEL_TEXT_OFFSET_Y,
			playerLevel_);
	}
	else
	{
		resumeButton_->Invisible();
		BackTitleButton_->Invisible();
	}

}

void PausePanel::Release()
{
}