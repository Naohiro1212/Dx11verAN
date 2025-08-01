cbuffer ConstantBuffer : register(b0)
{
    float time; // 時間（秒）
    float4x4 worldViewProj; // ワールドビュー射影行列
};

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 tex : TEXCOORD0;
};

struct PS_INPUT
{
    float4 position : SV_Position;
    float3 normal : NORMAL;
    float2 tex : TEXCOORD0;
};

float WaveHeight(float3 pos, float t)
{
    // 簡単な正弦波計算
    float wave1 = sin(pos.x * 0.1 + t) * 0.5;
    float wave2 = cos(pos.z * 0.15 + t * 1.3) * 0.3;
    return wave1 + wave2;
}

PS_INPUT VSMain(VS_INPUT input)
{
    PS_INPUT output;
    
    // 波の高さを計算して頂点位置のｙ座標を変更
    float height = WaveHeight(input.position, time);
    float3 pos = input.position;
    pos.y += height;
    
    // 変位後の位置をワールドビュー射影行列で変換
    output.position = mul(float4(pos, 1.0f), worldViewProj);
    
    // 法線は簡単に上向き固定
    output.normal = float(0, 1, 0);
    
    output.tex = input.tex; // UV座標はそのまま
    
    return output;
}

float4 PSMain(PS_INPUT input) : SV_Target
{
    // 簡単な色を返す（波の高さに応じて色を変えることも可能）
    float baseColor = float4(0.0f, 0.3f, 0.5f, 1.0f);
    
    // 簡単な光の反射ライクな表現
    float fresnel = pow(1.0f - dot(normalize(input.normal), float3(0, 1, 0)), 3.0f);

    float4 color = lerp(baseColor, float4(1.0f, 1.0f, 1.0f, 1.0f), saturate(fresnel));
    
    return color;
}