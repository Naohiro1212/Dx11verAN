#include "Texture.h"
#include "Direct3D.h"
#include "Global.h"
#include <vector>
#include <algorithm>
#include <cmath>

Texture::Texture():
	pSampleLinear_(nullptr), pTextureSRV_(nullptr), size_(XMFLOAT3(0,0,0))
{
}


Texture::~Texture()
{
	SAFE_RELEASE(pSampleLinear_);
	SAFE_RELEASE(pTextureSRV_);
}

HRESULT Texture::Load(std::string fileName)
{
	wchar_t wtext[FILENAME_MAX];
	size_t ret;
	mbstowcs_s(&ret, wtext, fileName.c_str(), fileName.length());


	// テクスチャを読み込む
	CoInitialize(NULL);
	IWICImagingFactory *pFactory = NULL;
	IWICBitmapDecoder *pDecoder = NULL;
	IWICBitmapFrameDecode* pFrame = NULL;
	IWICFormatConverter* pFormatConverter = NULL;
	CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, reinterpret_cast<void **>(&pFactory));
	HRESULT hr = pFactory->CreateDecoderFromFilename(wtext, NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &pDecoder);
	if(FAILED(hr))
	{
		char message[256];
		wsprintf(message, "「%s」が見つかりません", fileName.c_str());
		MessageBox(0, message, "画像ファイルの読み込みに失敗", MB_OK);
		return hr;
	}
	pDecoder->GetFrame(0, &pFrame);
	pFactory->CreateFormatConverter(&pFormatConverter);
	pFormatConverter->Initialize(pFrame, GUID_WICPixelFormat32bppRGBA, WICBitmapDitherTypeNone, NULL, 1.0f, WICBitmapPaletteTypeMedianCut);
	UINT imgWidth;
	UINT imgHeight;
	pFormatConverter->GetSize(&imgWidth, &imgHeight);
	size_ = XMFLOAT3((float)imgWidth, (float)imgHeight, 0);

	// テクスチャの設定
	ID3D11Texture2D*	pTexture;			// テクスチャデータ
	D3D11_TEXTURE2D_DESC texdec;
	texdec.Width = imgWidth;
	texdec.Height = imgHeight;
	texdec.MipLevels = 1;
	texdec.ArraySize = 1;
	texdec.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texdec.SampleDesc.Count = 1;
	texdec.SampleDesc.Quality = 0;
	texdec.Usage = D3D11_USAGE_DYNAMIC;
	texdec.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texdec.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texdec.MiscFlags = 0;
	Direct3D::pDevice_->CreateTexture2D(&texdec, NULL, &pTexture);

	// テクスチャを送る
	D3D11_MAPPED_SUBRESOURCE hMappedres;
	Direct3D::pContext_->Map(pTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &hMappedres);
	pFormatConverter->CopyPixels(NULL, imgWidth * 4, imgWidth * imgHeight * 4, (BYTE*)hMappedres.pData);
	Direct3D::pContext_->Unmap(pTexture, 0);


	// シェーダリソースビュー(テクスチャ用)の設定
	D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};
	srv.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv.Texture2D.MipLevels = 1;
	Direct3D::pDevice_->CreateShaderResourceView(pTexture, &srv, &pTextureSRV_);


	// テクスチャー用サンプラー作成
	D3D11_SAMPLER_DESC  SamDesc;
	ZeroMemory(&SamDesc, sizeof(D3D11_SAMPLER_DESC));
	SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	Direct3D::pDevice_->CreateSamplerState(&SamDesc, &pSampleLinear_);

	pTexture->Release();
	pFormatConverter->Release();
	pFrame->Release();
	pDecoder->Release();
	pFactory->Release();

	return S_OK;
}

HRESULT Texture::CreateSolidColor(UINT _width, UINT _height, const DirectX::XMFLOAT4& _color)
{
	if (_width == 0)
	{
		_width = 1;
	}
	if (_height == 0)
	{
		_height = 1;
	}

	size_ = XMFLOAT3(static_cast<float>(_width), static_cast<float>(_height), 0.0f);

	auto toByte = [](float f) -> uint8_t
		{
			f = std::clamp(f, 0.0f, 1.0f);
			return static_cast<uint8_t>(std::lround(f * 255.0f));
		};

	const uint8_t r = toByte(_color.x);
	const uint8_t g = toByte(_color.y);
	const uint8_t b = toByte(_color.z);
	const uint8_t a = toByte(_color.w);

	const uint32_t rgba = (static_cast<uint32_t>(r)) |
		(static_cast<uint32_t>(g) << 8) |
		(static_cast<uint32_t>(b) << 16) |
		(static_cast<uint32_t>(a) << 24);

	std::vector<uint32_t> pixels(static_cast<size_t>(_width) * _height, rgba);

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = _width;
	desc.Height = _height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA srd = {};
	srd.pSysMem = pixels.data();
	srd.SysMemPitch = _width * 4;

	ID3D11Texture2D* pTexture = nullptr;
	HRESULT hr = Direct3D::pDevice_->CreateTexture2D(&desc, &srd, &pTexture);
	if (FAILED(hr)) return hr;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
	srvd.Format = desc.Format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MipLevels = 1;
	hr = Direct3D::pDevice_->CreateShaderResourceView(pTexture, &srvd, &pTextureSRV_);

	if (FAILED(hr)) { pTexture->Release(); return hr; }

	D3D11_SAMPLER_DESC SamDesc = {};
	SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; // 単色ならPOINTで十分
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	hr = Direct3D::pDevice_->CreateSamplerState(&SamDesc, &pSampleLinear_);

	pTexture->Release();
	return hr;
}
