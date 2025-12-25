#include "RootObject.h"
#include "SceneManager.h"
#include "ScoreManager.h"


RootObject::RootObject():
	GameObject(nullptr, "RootObject")
{
}


RootObject::~RootObject()
{
}

void RootObject::Initialize()
{
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
