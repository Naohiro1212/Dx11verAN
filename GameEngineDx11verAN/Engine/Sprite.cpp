#include "Sprite.h"
#include "Direct3D.h"
#include "Global.h"

//コンストラクタ
Sprite::Sprite():
	pTexture_(nullptr)
{
}

//デストラクタ
Sprite::~Sprite()
{

	SAFE_RELEASE(pVertexBuffer_);
	SAFE_RELEASE(pIndexBuffer_);
}

//準備
HRESULT Sprite::Load(std::string fileName)
{
	//テクスチャ準備
	pTexture_ = new Texture();
	if(FAILED(pTexture_->Load(fileName)))
	{
		return E_FAIL;
	}

	//頂点情報準備
	InitVertex();

	//インデックス情報準備
	InitIndex();
	
	//コンスタントバッファ準備
	InitConstantBuffer();

	return S_OK;
}

HRESULT Sprite::LoadSolidColor(const DirectX::XMFLOAT4& color)
{
	pTexture_ = new Texture();
	if (FAILED(pTexture_->CreateSolidColor(1, 1, color)))
	{
		SAFE_DELETE(pTexture_);
		return E_FAIL;
	}
	InitVertex();
	InitIndex();
	InitConstantBuffer();
	return S_OK;
}

//コンスタントバッファ準備
void Sprite::InitConstantBuffer()
{
	//必要な設定項目
	D3D11_BUFFER_DESC cb;
	cb.ByteWidth =		sizeof(CONSTANT_BUFFER);
	cb.Usage =			D3D11_USAGE_DYNAMIC;
	cb.BindFlags =		D3D11_BIND_CONSTANT_BUFFER;
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags =		0;
	cb.StructureByteStride = 0;

	// 定数バッファの作成
	Direct3D::pDevice_->CreateBuffer(&cb, NULL, &pConstantBuffer_);
}


void Sprite::InitVertex()
{
	// 頂点データ宣言
	VERTEX vertices[] =
	{
		{ XMFLOAT3(-1.0f,  1.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f) },   // 四角形の頂点（左上）
		{ XMFLOAT3( 1.0f,  1.0f, 0.0f),	XMFLOAT3(1.0f, 0.0f, 0.0f) },   // 四角形の頂点（右上）
		{ XMFLOAT3(-1.0f, -1.0f, 0.0f),	XMFLOAT3(0.0f, 1.0f, 0.0f) },   // 四角形の頂点（左下）
		{ XMFLOAT3( 1.0f, -1.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 0.0f) },   // 四角形の頂点（右下）
	};


	// 頂点データ用バッファの設定
	D3D11_BUFFER_DESC bd_vertex;
	bd_vertex.ByteWidth = sizeof(vertices);
	bd_vertex.Usage = D3D11_USAGE_DEFAULT;
	bd_vertex.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd_vertex.CPUAccessFlags = 0;
	bd_vertex.MiscFlags = 0;
	bd_vertex.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA data_vertex;
	data_vertex.pSysMem = vertices;
	Direct3D::pDevice_->CreateBuffer(&bd_vertex, &data_vertex, &pVertexBuffer_);
}

void Sprite::InitIndex()
{
	int index[] = {2,1,0, 2,3,1 };

	// インデックスバッファを生成する
	D3D11_BUFFER_DESC   bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(index);
	bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = index;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	Direct3D::pDevice_->CreateBuffer(&bd, &InitData, &pIndexBuffer_);
}

// スプライトを描画する
// transform: スプライトのワールド座標
// rect     : スプライトのテクスチャ領域
// alpha    : スプライトの透明度(0.0f～1.0f)
void Sprite::Draw(Transform& transform, RECT rect, float alpha)
{
	//いろいろ設定
	Direct3D::SetShader(Direct3D::SHADER_2D);
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	Direct3D::pContext_->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);
	Direct3D::pContext_->VSSetConstantBuffers(0, 1, &pConstantBuffer_);
	Direct3D::pContext_->PSSetConstantBuffers(0, 1, &pConstantBuffer_);
	Direct3D::SetDepthBafferWriteEnable(false);

	// インデックスバッファーをセット
	stride = sizeof(int);
	offset = 0;
	Direct3D::pContext_->IASetIndexBuffer(pIndexBuffer_, DXGI_FORMAT_R32_UINT, 0);

	// パラメータの受け渡し
	D3D11_MAPPED_SUBRESOURCE pdata;
	CONSTANT_BUFFER cb;

	// スプライトの幅/高さ（ピクセル）
	float w = static_cast<float>(rect.right - rect.left);
	float h = static_cast<float>(rect.bottom - rect.top);

	// 頂点は -1..1 の中心原点で作ってあるため、
	// スケールは rect の"半分"を使う（幅=2 をピクセル幅へマップ）
	XMMATRIX cut = XMMatrixScaling(w * 0.5f, h * 0.5f, 1.0f);

	// transform.position_ は左上座標を想定しているので、
	// 描画のために中心座標へ変換する（center = top-left + (w/2,h/2)）
	float centerX = transform.position_.x + w * 0.5f;
	float centerY = transform.position_.y + h * 0.5f;
	XMMATRIX trans = XMMatrixTranslation(centerX, centerY, transform.position_.z);

	// 画面への直交投影（左上を (0,0) としたいので top=0, bottom=screenHeight）
	XMMATRIX proj = XMMatrixOrthographicOffCenterLH(
		0.0f, (float)Direct3D::screenWidth_,
		(float)Direct3D::screenHeight_, 0.0f,
		0.0f, 1.0f);

	// モデル行列（切り取りスケール → スケール/回転 → 平行移動(center)）
	XMMATRIX model = cut * transform.matScale_ * transform.matRotate_ * trans;

	// ワールド行列 = model * projection (DirectX は行列の掛け順に注意)
	XMMATRIX world = model * proj;
	cb.world = XMMatrixTranspose(world);

	// テクスチャ座標変換行列を渡す
	XMMATRIX mTexTrans = XMMatrixTranslation((float)rect.left / (float)pTexture_->GetSize().x,
		(float)rect.top / (float)pTexture_->GetSize().y, 0.0f);
	XMMATRIX mTexScale = XMMatrixScaling((float)rect.right / (float)pTexture_->GetSize().x,
		(float)rect.bottom / (float)pTexture_->GetSize().y, 1.0f);
	XMMATRIX mTexel = mTexScale * mTexTrans;
	cb.uvTrans = XMMatrixTranspose(mTexel);
	
	// テクスチャ合成色情報を渡す
	cb.color = XMFLOAT4(1, 1, 1, alpha);

	Direct3D::pContext_->Map(pConstantBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata);	// GPUからのリソースアクセスを一時止める
	memcpy_s(pdata.pData, pdata.RowPitch, (void*)(&cb), sizeof(cb));		// リソースへ値を送る

	ID3D11SamplerState*			pSampler = pTexture_->GetSampler();
	Direct3D::pContext_->PSSetSamplers(0, 1, &pSampler);

	ID3D11ShaderResourceView*	pSRV = pTexture_->GetSRV();
	Direct3D::pContext_->PSSetShaderResources(0, 1, &pSRV);

	Direct3D::pContext_->Unmap(pConstantBuffer_, 0);									// GPUからのリソースアクセスを再開

	//ポリゴンメッシュを描画する
	Direct3D::pContext_->DrawIndexed(6, 0, 0);

	Direct3D::SetShader(Direct3D::SHADER_3D);

	Direct3D::SetDepthBafferWriteEnable(true);
}

// スプライトを描画する（transform_版）
void Sprite::Draw(RECT rect, float alpha)
{
	Draw(transform_, rect, alpha);
}
