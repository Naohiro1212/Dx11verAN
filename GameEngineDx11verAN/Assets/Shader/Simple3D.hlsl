//───────────────────────────────────────
 // テクスチャ＆サンプラーデータのグローバル変数定義
//───────────────────────────────────────
Texture2D		g_texture: register(t0);	//テクスチャー
SamplerState	g_sampler : register(s0);	//サンプラー

//───────────────────────────────────────
 // コンスタントバッファ
// DirectX 側から送信されてくる、ポリゴン頂点以外の諸情報の定義
//───────────────────────────────────────
cbuffer global
{
	float4x4	g_matWVP;			// ワールド・ビュー・プロジェクションの合成行列
	float4x4	g_matNormalTrans;	// 法線の変換行列（回転行列と拡大の逆行列）
	float4x4	g_matWorld;			// ワールド変換行列
	float4		g_vecLightDir;		// ライトの方向ベクトル
	float4		g_vecDiffuse;		// ディフューズカラー（マテリアルの色）
	float4		g_vecAmbient;		// アンビエントカラー（影の色）
	float4		g_vecSpeculer;		// スペキュラーカラー（ハイライトの色）
	float4		g_vecCameraPosition;// 視点（カメラの位置）
	float		g_shuniness;		// ハイライトの強さ（テカリ具合）
	bool		g_isTexture;		// テクスチャ貼ってあるかどうか
};

//───────────────────────────────────────
// 頂点シェーダー出力＆ピクセルシェーダー入力データ構造体
//───────────────────────────────────────
struct VS_OUT
{
	float4 pos    : SV_POSITION;	//位置
	float4 normal : TEXCOORD2;		//法線
	float2 uv	  : TEXCOORD0;		//UV座標
	float4 eye	  : TEXCOORD1;		//視線
};

//───────────────────────────────────────
// 頂点シェーダ
//───────────────────────────────────────
VS_OUT VS(float4 pos : POSITION, float4 Normal : NORMAL, float2 Uv : TEXCOORD)
{
	//ピクセルシェーダーへ渡す情報
	VS_OUT outData;

	//ローカル座標に、ワールド・ビュー・プロジェクション行列をかけて
	//スクリーン座標に変換し、ピクセルシェーダーへ
	outData.pos = mul(pos, g_matWVP);		

	//法線の変形
	Normal.w = 0;					//4次元目は使わないので0
	Normal = mul(Normal, g_matNormalTrans);		//オブジェクトが変形すれば法線も変形
	outData.normal = Normal;		//これをピクセルシェーダーへ

	//視線ベクトル（ハイライトの計算に必要
	float4 worldPos = mul(pos, g_matWorld);					//ローカル座標にワールド行列をかけてワールド座標へ
	outData.eye = normalize(g_vecCameraPosition - worldPos);	//視点から頂点位置を引き算し視線を求めてピクセルシェーダーへ

	//UV「座標
	outData.uv = Uv;	//そのままピクセルシェーダーへ


	//まとめて出力
	return outData;
}

//───────────────────────────────────────
// ピクセルシェーダ
//───────────────────────────────────────
float4 PS(VS_OUT inData) : SV_Target
{
	//ライトの向き
	float4 lightDir = g_vecLightDir;	//グルーバル変数は変更できないので、いったんローカル変数へ
	lightDir = normalize(lightDir);	//向きだけが必要なので正規化

	//法線はピクセルシェーダーに持ってきた時点で補完され長さが変わっている
	//正規化しておかないと面の明るさがおかしくなる
	inData.normal = normalize(inData.normal);

	//拡散反射光（ディフューズ）
	//法線と光のベクトルの内積が、そこの明るさになる
    float minShade = 0.2f; // 最低限の明るさを設定
	float4 shade = saturate(dot(inData.normal, -lightDir));
    shade = max(shade, minShade);
	shade.a = 1;	//暗いところが透明になるので、強制的にアルファは1

	float4 diffuse;
	//テクスチャ有無
	if (g_isTexture == true)
	{
		//テクスチャの色
		diffuse = g_texture.Sample(g_sampler, inData.uv);
	}
	else
	{
		//マテリアルの色
	//	diffuse = g_vecDiffuse;
        diffuse = float4(1, 1, 1, 1);
    }

	//環境光（アンビエント）
	//これはMaya側で指定し、グローバル変数で受け取ったものをそのまま
    float ambientStrength = 0.3f;
	float4 ambient = g_vecAmbient * ambientStrength;

	//鏡面反射光（スペキュラー）金属寄り
    float3 N = normalize(inData.normal.xyz);
    float3 V = normalize(inData.eye.xyz);
    float3 L = normalize((-g_vecLightDir).xyz);
    float3 H = normalize(L + V);

	// ベースのスペキュラーカラー（やや青みの白）
    float3 specColor = g_vecSpeculer.rgb; // 例: (0.9, 0.95, 1.0)
	// Blinn-Phong ローブ（鋭さは g_shuniness）
    float spec = pow(saturate(dot(N, H)), g_shuniness);

	// 簡易Fresnel（Schlick近似）
    float F0 = 0.16; // 金属感ベース（0.04?0.2程度）材質で調整
    float oneMinusVoN = 1.0 - saturate(dot(V, N));
    float fresnel = F0 + (1.0 - F0) * pow(oneMinusVoN, 5.0);

	// スペキュラー強度（距離減衰なしの場合）
    float specIntensity = 1.2; // 全体強度ノブ（0.5?1.5で調整）

    float3 speculerRGB = specColor * spec * fresnel * specIntensity;
    float4 speculer = float4(speculerRGB, 1.0);

	//最終的な色
    float diffuseScale = 0.8f; // ディフューズの影響度調整
    float4 color = diffuse * shade * diffuseScale + diffuse * ambient + speculer;
    return color;
}