#include "Sword.h"
#include "../Engine/Model.h"

Sword::Sword(GameObject* parent) : GameObject(parent), swordModel_(-1)
{
}

void Sword::Initialize()
{
	swordModel_ = Model::Load("Models/sword.fbx");
}

void Sword::Update()
{
	
}

void Sword::Draw()
{
}

void Sword::Release()
{
}
