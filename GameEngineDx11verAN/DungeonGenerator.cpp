#include "DungeonGenerator.h"
#include "Engine/Model.h"

DungeonGenerator::DungeonGenerator(GameObject* parent)
	: GameObject(parent, "TestScene"), hBoxModel_(-1)
{

}

DungeonGenerator::~DungeonGenerator()
{
}

void DungeonGenerator::Initialize()
{
	hBoxModel_ = Model::Load("Cube.fbx");
	assert(hBoxModel_ >= 0);
}

void DungeonGenerator::Update()
{
}

void DungeonGenerator::Draw()
{
}

void DungeonGenerator::Release()
{
}

void DungeonGenerator::GenerateDungeon(int _dWidth, int _dHeight)
{
	// ƒ_ƒ“ƒWƒ‡ƒ“‚Ì•‚Æ‰œs‚ğİ’è
	dungeonWidth_ = _dWidth;
	dungeonHeight_ = _dHeight;
	

}
