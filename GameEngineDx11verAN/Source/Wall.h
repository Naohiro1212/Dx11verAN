#pragma once
#include "../Engine/GameObject.h"

class Wall
	:public GameObject
{
public:
	Wall(GameObject* _parent, XMFLOAT3 _pos);
	~Wall();

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release() override;

private:
	int hModel_;
};