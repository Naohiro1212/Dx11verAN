#include "PausePanel.h"
#include "../Engine/Image.h"
#include "../Engine/Direct3D.h"
#include <assert.h>
#include "../Engine/Button.h"
#include "../Engine/Input.h"
#include "../Source/Player.h"
#include "../Engine/Text.h"

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
	Image::SetPositionPixels(panelImage_, Direct3D::screenWidth_ * 0.5f, Direct3D::screenHeight_ * 0.5f, true);

	// ボタン初期化
	resumeButton_ = Instantiate<Button>(this);
	resumeButton_->SetCenter(true);
	resumeButton_->SetButtonImage(Image::Load("ResumeButton.png"));
	resumeButton_->SetButtonPosition(Direct3D::screenWidth_ * 0.5f, Direct3D::screenHeight_ * 0.5f - 50.0f);

	BackTitleButton_ = Instantiate<Button>(this);
	BackTitleButton_->SetCenter(true);
	BackTitleButton_->SetButtonImage(Image::Load("BackTitleButton.png"));
	BackTitleButton_->SetButtonPosition(Direct3D::screenWidth_ * 0.5f, Direct3D::screenHeight_ * 0.5f + 120.0f);

	// レベルテキスト初期化
	player_ = dynamic_cast<Player*>(FindObject("Player"));
	levelText_ = new Text();
	levelText_->Initialize();
	levelText_->SetScale(1.7f);

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
		levelText_->Draw(Direct3D::screenWidth_ * 0.5f - 100.0f, Direct3D::screenHeight_ * 0.5f - 180.0f, "LEVEL:");
		levelText_->Draw(Direct3D::screenWidth_ * 0.5f + 100.0f, Direct3D::screenHeight_ * 0.5f - 180.0f, playerLevel_);
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