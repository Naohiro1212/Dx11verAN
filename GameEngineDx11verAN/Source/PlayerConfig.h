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
    const float BASE_ANIM_FPS = 60.0f;     // モーションが想定する基準FPS（仮定）
    const float SLASH_PLAY_SPEED = 0.9f;      // 既存指定の再生スピード
    // 実時間 = (フレーム数 / FPS) / 再生スピード
    const float SLASH_DURATION_SEC = (SLASH_ANIM_END - SLASH_ANIM_START + 1) / BASE_ANIM_FPS / SLASH_PLAY_SPEED;

    // アニメーションそれぞれの速度
    const float ANIM_BASE_SPEED = 0.5f;
    const int ANIM_BASE_START = 1;
    const int ANIM_IDLE_END = 76;
    const int ANIM_STRAFE_END = 21;
    const int ANIM_WALK_END = 32;
    const int ANIM_BACK_END = 17;
    const int ANIM_JUMP_END = 60;
    const int ANIM_DEATH_END = 230;
    // 死亡モーションの猶予の値
    const int ANIM_DEATH_BUFFER = 5;
	const float ANIM_JUMP_BUFFER = 0.12f; // ジャンプアニメーションの速度調整用
    const float ANIM_DEATH_PLAY_SPEED = 1.2f;

    // プレイヤーのスケール
    const float PLAYER_SCALE = 0.1f;

    // ボディコライダーのオフセット
    const XMFLOAT3 COLLIDER_BASE_POS = XMFLOAT3(0.0f, 10.0f, 0.0f);
    const XMFLOAT3 COLLIDER_SCALE = XMFLOAT3(PLAYER_SCALE * 80.0f, PLAYER_SCALE * 160.0f, PLAYER_SCALE * 70.0f);

    // 攻撃コライダーのオフセット
	const XMFLOAT3 ATTACK_COLLIDER_BASE_POS = XMFLOAT3(0.0f, 0.0f, 0.0f);
	const XMFLOAT3 ATTACK_COLLIDER_SCALE = XMFLOAT3(PLAYER_SCALE * 100.0f, PLAYER_SCALE * 80.0f, PLAYER_SCALE * 140.0f);

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

    // 近接攻撃位置のオフセット
	const float ATTACK_COLLIDER_FORWARD_OFFSET = 90.0f;
    const float ATTACK_COLLIDER_HEIGHT_OFFSET = 100.0f;

    // レイキャスト用の変数
    const float PROBE_UP_OFFSET = 1.0f;
    const float ENTER_GROUND_EPS = 0.015f;
    const float EXIT_GROUND_EPS = 0.03f;
	const XMFLOAT3 RAY_DIR = XMFLOAT3(0.0f, -10.0f, 0.0f);

    // 壁当たり判定関連
    const float WALL_EPS = 1e-3f;

    // 魔力ゲージ関連
    const float MAX_MANA = 100.0f;
	const float MANA_RECOVERY_RATE = 7.0f; // 1秒
    
	// 最低マナ10（魔法発射に必要）
	const float MAGIC_MANA_COST = 25.0f;   

    // 体力関係
	const float MAX_HEALTH = 100.0f;

    // ダメージを受けたときの無敵時間
	const float DAMAGE_INVINCIBLE_TIME = 1.0f;

    // 効果音が鳴るまでのディレイ
	const float SLASH_SOUND_DELAY = 0.5f;

    // Audioのマスターボリューム
	const float MASTER_VOLUME = 0.1f;

	// ジャンプの初速度を求めるための定数
	const float JUMP_V0_CONSTANT = 2.0f * GRAVITY * JUMP_HEIGHT;

    // 閾値のための極小値
    const float EPSILON = 1e-6f;

	// ダッシュの速度倍率
	const float DASH_MULTIPLIER = 1.8f;

	// 壁との当たり判定の値
	// cos(66度) = 0.4f なので、これより小さいときはほぼ垂直な壁とみなす
    const float WALL_SLIDE_MAX_NORMAL_Y = 0.4f;

    // 影のスケール
    const XMFLOAT3 SHADOW_SCALE = XMFLOAT3(10.0f, 20.0f, 16.0f);
    // 影のyオフセット
	const float SHADOW_OFFSET_Y = 0.1f;
    const XMFLOAT4 SHADOW_COLOR = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.4f); // 黒＋40%透明

    // ジュエル1個あたりの経験値
	const int JEWEL_EXP = 20; 

	// レベルアップで上昇するステータスの値
    const float LEVELUP_STRENGTH = 5.0f;
};