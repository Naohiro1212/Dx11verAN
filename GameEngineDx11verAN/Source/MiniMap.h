#pragma once
#include "../Engine/GameObject.h"

// D3Dの前方宣言
struct ID3D11DepthStencilState;

class DungeonManager;

class MiniMap : public GameObject
{
public:
	MiniMap(GameObject* parent);
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release() override;

private:
	DungeonManager* dungeonManager_;
	int wallModel_;
	int playerModel_;

	// ミニマップ描画用：深度テスト無効ステート
	ID3D11DepthStencilState* noDepthState_;

	Transform plTransform_;
	Transform enTransform_;

};