#include "AttackCollider.h"
#include "../Engine/BoxCollider.h"
#include "../Engine/Model.h"

AttackCollider::AttackCollider(GameObject* parent) : GameObject(parent), pCollider_(nullptr), debugModel_(-1)
{
}

void AttackCollider::Initialize()
{
#ifdef _DEBUG
	debugModel_ = Model::Load("Models/cube.fbx");
#endif

	pCollider_ = new BoxCollider();
}

void AttackCollider::Update()
{
	
}

void AttackCollider::Draw()
{
#ifdef _DEBUG
	Model::SetTransform(debugModel_, transform_);
	Model::Draw(debugModel_);
#endif
}

void AttackCollider::Release()
{
}
