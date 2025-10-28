#pragma once
#include "../Engine/GameObject.h"

// D3Dの前方宣言
struct ID3D11DepthStencilState;

class DungeonManager;

class MiniMap : public GameObject
{
public:
	MiniMap(GameObject* parent);
	~MiniMap();
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release() override;

	// 矩形の更新
	void SetRect(int _x, int _y, int _w, int _h);

private:
	DungeonManager* dungeonManager_;
	int wallModel_;
	int playerModel_;
	int frameImage_;

	Transform plTransform_;
	Transform enTransform_;
};