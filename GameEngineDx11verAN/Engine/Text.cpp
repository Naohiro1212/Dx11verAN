#include <stdlib.h>
#include "Direct3D.h"
#include "Text.h"
#include "../Engine/Debug.h"

Text::Text() : hPict_(-1), width_(16), height_(32), fileName_("char.png"), rowLength_(16)
{
}

Text::~Text()
{
}

//初期化（デフォルト）
HRESULT Text::Initialize()
{
	hPict_ = Image::Load(fileName_);
	assert(hPict_ >= 0);

	if (hPict_ < 0)
		return E_FAIL;

	return S_OK;
}

//初期化（オリジナルの画像）
HRESULT Text::Initialize(const char* fileName, const unsigned int charWidth, const unsigned int charHeight, const unsigned int rowLength)
{
	strcpy_s(fileName_, fileName);
	width_ = charWidth;
	height_ = charHeight;
	rowLength_ = rowLength;
	return Initialize();
}

// 描画（文字列）
void Text::Draw(int x, int y, const char* str)
{
    // 表示開始位置（ピクセル、左上原点）
    float px = (float)x;
    float py = (float)y;

    const char baseChar = '!'; // フォント画像に合わせて変更
    const int maxChars = rowLength_ * 16; // 十分大きめの上限（適宜調整）

    for (int i = 0; str[i] != '\0'; ++i)
    {
        unsigned char ch = static_cast<unsigned char>(str[i]);
        int id = (int)ch - (int)baseChar;

        // 範囲外の文字は描画せず幅分だけ進める（スペース等の扱い）
        if (id < 0 || id >= maxChars)
        {
            px += (float)width_;
            continue;
        }

        int cellX = id % rowLength_; // 左から何番目
        int cellY = id / rowLength_; // 上から何番目

        // 表示する位置（ピクセル座標をそのまま渡す）
        Transform transform;
        transform.matScale_ = XMMatrixIdentity();
		transform.matRotate_ = XMMatrixIdentity();
        transform.position_.x = px;
        transform.position_.y = py;
		transform.position_.z = 0.0f;
        Image::SetTransform(hPict_, transform);

        // Image::SetRect の実装は (handle, x, y, width, height) を期待するので合わせる
        Image::SetRect(hPict_, width_ * cellX, height_ * cellY, width_, height_);

        RECT r = Image::GetRect(hPict_);

        // 表示
        Image::Draw(hPict_);

        // 次の位置（ピクセル単位）
        //px += (float)width_;
    }
}

//描画（整数値）
void Text::Draw(int x, int y, int value)
{
	//文字列に変換
	char str[256];
	sprintf_s(str, "%d", value);

	Draw(x, y, str);
}

//解放
void Text::Release()
{
	Image::Release(hPict_);
}
