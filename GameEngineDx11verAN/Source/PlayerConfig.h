#pragma once
#include <DirectXMath.h>
#include <cstddef>
using namespace DirectX;

struct PlayerConfig
{
    const float PLAYER_SPEED = 30.0f; // プレイヤーの移動速度
    const float PLAYER_ROTATE_SPEED = 30.0f; // プレイヤーの回転速度

    // カメラのマウス移動初期値
    const float VISION_INIT_YAW_DEG = 180.0f;
    const float VISION_INIT_PITCH_DEG = 20.0f;
    const float VISION_INIT_DISTANCE = 20.0f;

    // カメラの初期値
    const float CAMERA_INIT_POS_Y = 10.0f;
    const float CAMERA_INIT_POS_Z = -20.0f;

    const float GRAVITY = 18.0f;
    const float JUMP_HEIGHT = 12.0f;
    const size_t JUMP_MAX_COUNT = 1;

    // カメラの逆方向にする
    const float FACE_OFFSET_DEG = 180.0f;
    // 1秒当たりのターン率
    const float TURN_SPEED_DEG = 540.0f;

    // スラッシュアニメーション関連
    const int   SLASH_ANIM_START = 1;
    const int   SLASH_ANIM_END = 45;
    const float BASE_ANIM_FPS = 30.0f;     // モーションが想定する基準FPS（仮定）
    const float SLASH_PLAY_SPEED = 0.65f;      // 既存指定の再生スピード
    // 実時間 = (フレーム数 / FPS) / 再生スピード
    const float SLASH_DURATION_SEC = (SLASH_ANIM_END - SLASH_ANIM_START + 1) / BASE_ANIM_FPS / SLASH_PLAY_SPEED;

    // アニメーションそれぞれの速度
    const float ANIM_BASE_SPEED = 0.5f;
    const int ANIM_BASE_START = 1;
    const int ANIM_IDLE_END = 76;
    const int ANIM_STRAFE_END = 21;
    const int ANIM_WALK_END = 32;
    const int ANIM_BACK_END = 17;

    // プレイヤーのスケール
    const float PLAYER_SCALE = 0.1f;

    // ボディコライダーのオフセット
    const XMFLOAT3 COLLIDER_BASE_POS = XMFLOAT3(0.0f, 10.0f, 0.0f);
    const XMFLOAT3 COLLIDER_SCALE = XMFLOAT3(PLAYER_SCALE * 40.0f, PLAYER_SCALE * 170.0f, PLAYER_SCALE * 40.0f);

    // 攻撃コライダーのオフセット
	const XMFLOAT3 ATTACK_COLLIDER_BASE_POS = XMFLOAT3(0.0f, PLAYER_SCALE * 100.0f, PLAYER_SCALE * 50.0f);
	const XMFLOAT3 ATTACK_COLLIDER_SCALE = XMFLOAT3(PLAYER_SCALE * 60.0f, PLAYER_SCALE * 100.0f, PLAYER_SCALE * 100.0f);

	// 円周率関連
	const float HALF_TURN = 180.0f;
    const float FULL_TURN = 360.0f;

    // 重力加速度
    const float GRAVITY_MULTIPLIER = 2.0f;

	// 魔法弾の生成オフセット
	const XMFLOAT3 MAGIC_SPHERE_SPAWN_OFFSET = XMFLOAT3(60.0f, 100.0f, 60.0f);

    // 発射位置のオフセット
    const float FORWARDDIST_OFFSET = 50.0f;
    const float HEIGHT_OFFSET = 100.0f;

    // レイキャスト用の変数
    const float PROBE_UP_OFFSET = 2.0f;
    const float GROUND_EPS = 0.05f;

    // 壁当たり判定関連
    const float WALL_EPS = 1e-3f;
};