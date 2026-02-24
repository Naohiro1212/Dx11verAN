#include "PlayerMovement.h"
#include "../Source/Plane.h"
#include "../Engine/GameObject.h"
#include "../Engine/GameTime.h"
#include "../Engine/Input.h"

PlayerMovement::PlayerMovement(PlayerConfig cnf, Transform& transform, Plane* pPlane)
	: cnf_(cnf), transform_(transform), pPlane_(pPlane)
{

}

PlayerMovement::~PlayerMovement()
{
}

void PlayerMovement::Initialize()
{
	transform_.position_ = { 0.0f, 0.0f, 0.0f };
	transform_.rotate_ = { 0.0f, 0.0f, 0.0f };

	JumpV0_ = sqrtf(cnf_.JUMP_V0_CONSTANT);
	velocityY_ = 0.0f;

	jumpCount_ = 0;
	onGround_ = true;
}

void PlayerMovement::Update(
    bool isAttacking, 
    float health, 
    const XMFLOAT3& cameraForward,
    const std::vector<BoxCollider*>& wallColliders
)
{
	dt_ = GameTime::DeltaTime();

    // 1. 入力取得
    MoveInput();

    // 2. カメラ方向に回転補正
	UpdateYawToCamera(cameraForward, transform_.rotate_.y);

	// 3. ジャンプ処理
	Jump();

    // 4. 移動処理
    UpdateMovement();

    // 5. 壁判定・スライド
    ResolveWallCollisions(wallColliders);

    // 6. 重力処理
    UpdateGravity();
}

void PlayerMovement::ResolveWallCollisions(std::vector<BoxCollider*> wallColliders)
{
    // ダンジョンの壁との当たり判定
	// 当たっていたら、壁に沿ってスライドさせる
    for (auto* wallCollider_ : wallColliders)
    {
		PenetrationResult res = Collider::ComputeBoxVsBoxPenetration(pCollider_, wallCollider_);
        if (res.overlapped)
        {
			transform_.position_.x += res.push.x + (res.push.x > 0 ? cnf_.WALL_EPS : (res.push.x < 0 ? -cnf_.WALL_EPS : 0.0f));
			transform_.position_.z += res.push.z + (res.push.z > 0 ? cnf_.WALL_EPS : (res.push.z < 0 ? -cnf_.WALL_EPS : 0.0f));
            if (fabsf(res.normal.y) < cnf_.WALL_SLIDE_MAX_NORMAL_Y)
            {
                moveVec = SlideAlongWall(moveVec, res.normal);
            }
        }
    }
}

void PlayerMovement::UpdateYawToCamera(const XMFLOAT3& cameraForward, float& playerYaw)
{
    // カメラ前方ベクトル（XZ）をDirectXMathで取得済みとする
    XMVECTOR vForward = XMLoadFloat3(&cameraForward);

    // プレイヤーの現在の向き（Y軸回転）をラジアンで取得
    float currentYawRad = XMConvertToRadians(playerYaw);

    // プレイヤーの前方ベクトル（XZ, Y=0）
    XMVECTOR vPlayerForward = XMVectorSet(-sinf(currentYawRad), 0.0f, -cosf(currentYawRad), 0.0f);

    // カメラ前方ベクトルの向きから目標ヨー角を計算
    float targetYawRad = atan2f(cameraForward.x, cameraForward.z); // DirectXMathにもXMVectorGetX/Yはあるが、ここはfloatでOK
    float targetYawDeg = XMConvertToDegrees(targetYawRad) + cnf_.FACE_OFFSET_DEG;

    // 差分計算
    float diff = targetYawDeg - playerYaw;
    // -180~180 に折り返し
    while (diff > cnf_.HALF_TURN) diff -= cnf_.FULL_TURN;
    while (diff < -cnf_.HALF_TURN) diff += cnf_.FULL_TURN;

    float step = cnf_.TURN_SPEED_DEG * dt_;
    if (fabsf(diff) <= step)
    {
        playerYaw = targetYawDeg;
    }
    else
    {
        playerYaw += (diff > 0 ? step : -step);
    }
}

void PlayerMovement::MoveInput()
{
    // 入力を +1/0/-1 に畳む（カメラ相対移動: W/S=前後, A/D=ストレイフ）
	moveDir_ = { 0, 0 };

    // 攻撃しておらず、なおかつ死んでいない状態でのみ移動入力を受け付ける
    if (Input::IsKey(DIK_W)) 
    {
        moveDir_.fwd_ += 1;
    }
    if (Input::IsKey(DIK_S))
    {
        moveDir_.fwd_ -= 1;
    }
    if (Input::IsKey(DIK_D))
    {
        moveDir_.str_ += 1;
    }
    if (Input::IsKey(DIK_A))
    {
        moveDir_.str_ -= 1;
    }

    isMovingNow_ = false;
    if (moveDir_.fwd_ != 0 || moveDir_.str_ != 0) {
        isMovingNow_ = true;
    }
}

void PlayerMovement::Jump()
{
    if (Input::IsKeyDown(DIK_SPACE) && (onGround_ || jumpCount_ < cnf_.JUMP_MAX_COUNT))
    {
		velocityY_ = JumpV0_;
		onGround_ = false;
		jumpCount_++;
    }
}

void PlayerMovement::UpdateMovement()
{
    // カメラ相対の移動ベクトルで移動
    // ジャンプ中は、現在の移動方向を維持したまま、XZ平面で移動する
    // 空中制御は完全不可能にする
    XMVECTOR vMove = XMVectorZero();

    // 地上入力から方向ベクトル（XZ）を作る
    XMVECTOR vInput = XMVectorZero();
    if (fwd_ != 0)
    {
        vInput = XMVectorAdd(vInput, XMVectorScale(vForward, static_cast<float>(fwd_)));
    }
    if (str_ != 0)
    {
        vInput = XMVectorAdd(vInput, XMVectorScale(vRight, static_cast<float>(str_)));
    }
    // XZ平面へ投影（y=0）
    vInput = XMVectorSet(XMVectorGetX(vInput), 0.0f, XMVectorGetZ(vInput), 0.0f);

    // 地上→空中の遷移検出
    bool justLeftGround = (prevOnGround_ && !onGround_);

    if (!onGround_)
    {
        if (justLeftGround)
        {
            // 空中に出た瞬間に現在の移動方向をロック（無入力ならゼロ）
            if (XMVectorGetX(XMVector3LengthSq(vInput)) > cnf_.EPSILON && wasMoving_)
            {
                vAirMove_ = XMVector3Normalize(vInput);
            }
            else
            {
                vAirMove_ = XMVectorZero();
            }
        }

        // 空中では入力を無視してロック方向のみで移動
        vMove = vAirMove_;
    }
    else
    {
        // 地上にいるときは入力に応じて移動
        vMove = vInput;
        // wasMoving_ の更新（無入力判定）
        wasMoving_ = (XMVectorGetX(XMVector3LengthSq(vInput)) > cnf_.EPSILON);
    }

    // 次フレーム用に接地状態を保持
    prevOnGround_ = onGround_;

    // 正規化
    if (XMVector3LengthSq(vMove).m128_f32[0] > cnf_.EPSILON)
    {
        vMove = XMVector3Normalize(vMove);
    }
    XMFLOAT3 moveVec;
    XMStoreFloat3(&moveVec, vMove);

    // 移動処理
    // 接地状態でSHIFTキーでダッシュ（速度2倍）
    if (Input::IsKey(DIK_LSHIFT) && onGround_)
    {
        moveVec.x *= cnf_.DASH_MULTIPLIER;
        moveVec.z *= cnf_.DASH_MULTIPLIER;
    }
    transform_.position_.x += moveVec.x * cnf_.PLAYER_SPEED * dt_;
    transform_.position_.z += moveVec.z * cnf_.PLAYER_SPEED * dt_;
}

void PlayerMovement::UpdateGravity()
{
    const float ENTER_EPS = cnf_.ENTER_GROUND_EPS;
    const float EXIT_EPS = cnf_.EXIT_GROUND_EPS;

    // 重力（上昇/下降で倍率を切り替え）
    float g = cnf_.GRAVITY * (velocityY_ < 0.0f ? cnf_.GRAVITY_MULTIPLIER : 1.0f);

    // 次フレームの速度・位置を予測
    float nextVelY = velocityY_ - g * dt_;
    float nextY = transform_.position_.y + velocityY_ * dt_ - 0.5f * g * dt_ * dt_;

    // レイを上方オフセット位置から下向きに飛ばして地面を探す
    RayCastData hitData;
    hitData.start = transform_.position_;
    hitData.start.y += cnf_.PROBE_UP_OFFSET;
    hitData.dir = cnf_.RAY_DIR;
    Model::RayCast(pPlane_->GetPlaneHandle(), hitData);

    landedThisFrame = false;

    if (hitData.hit)
    {
        float groundY = hitData.start.y - hitData.dist;
        float maxTravel = (std::max)(0.5f, hitData.start.y - nextY);
        float threshold = onGround_ ? EXIT_EPS : ENTER_EPS;

        // 今フレーム内に床があり、nextY が閾値以内ならスナップして着地
        if (hitData.dist <= maxTravel && nextY <= groundY + threshold)
        {
            transform_.position_.y = groundY;
            velocityY_ = 0.0f;

            // 非接地→接地の遷移なら着地音
            if (!prevOnGround_) {
                landedThisFrame = true;
            }

            onGround_ = true;
            jumpCount_ = 0;
        }
        else
        {
            // スナップしない場合の onGround 判定（微小誤差は EXIT_EPS で緩和）
            bool wasOnGround = onGround_;
            onGround_ = (nextY <= groundY + EXIT_EPS);

            // 非接地→接地の遷移なら着地音
            if (!prevOnGround_ && onGround_) {
                landedThisFrame = true;
            }

            // 空中移動を適用
            transform_.position_.y = nextY;
            velocityY_ = nextVelY;
        }
    }
    else
    {
        onGround_ = false;

        // 空中移動を適用
        transform_.position_.y = nextY;
        velocityY_ = nextVelY;
    }

    // 接地状態での下向き速度のゼロ化（貫通防止）
    if (onGround_ && velocityY_ < 0.0f)
    {
        velocityY_ = 0.0f;
    }
}

XMFLOAT3 PlayerMovement::SlideAlongWall(const XMFLOAT3& f, const XMFLOAT3& n)
{
    XMVECTOR vf = XMLoadFloat3(&f);
    XMVECTOR vn = XMLoadFloat3(&n);

    // Y成分をゼロにして水平法線へ
    vn = XMVectorSet(XMVectorGetX(vn), 0.0f, XMVectorGetZ(vn), 0.0f);
    vn = XMVector3Normalize(vn);

    float d = XMVectorGetX(XMVector3Dot(vf, vn));
    XMVECTOR vw = XMVectorSubtract(vf, XMVectorScale(vn, d));

    XMFLOAT3 w;
    XMStoreFloat3(&w, vw);
    return w;
}
