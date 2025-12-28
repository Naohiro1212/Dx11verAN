#include "RootObject.h"
#include "SceneManager.h"
#include "ScoreManager.h"
#include "Image.h"

RootObject::RootObject():
	GameObject(nullptr, "RootObject")
{
}


RootObject::~RootObject()
{
}

void RootObject::Initialize()
{
	// Image‚ÍÅ‰‚É‰Šú‰»‚µ‚Ä‚¨‚­
	Image::Initialize();

	Instantiate<SceneManager>(this);
	Instantiate<ScoreManager>(this);
}

void RootObject::Update()
{
}

void RootObject::Draw()
{
}

void RootObject::Release()
{
}
