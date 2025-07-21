#include "Global.h"
#include "Model.h"

namespace Model
{
    std::vector<ModelData*> _datas;

    void Initialize()
    {
        AllRelease();
    }

    int Load(std::string fileName)
    {
        ModelData* pData = new ModelData;

        bool isExist = false;
        for (int i = 0; i < _datas.size(); i++)
        {
            if (_datas[i] != nullptr && _datas[i]->fileName == fileName)
            {
                pData->pFbx = _datas[i]->pFbx;
                isExist = true;
                break;
            }
        }
        if (isExist == false)
        {
            pData->pFbx = new Fbx;
            if (FAILED(pData->pFbx->Load(fileName)))
            {
                SAFE_DELETE(pData->pFbx);
                SAFE_DELETE(pData);
                return -1;
            }
            pData->fileName = fileName;
        }
        for (int i = 0; i < _datas.size(); i++)
        {
            if (_datas[i] == nullptr)
            {
                _datas[i] = pData;
                return i;
            }
        }
        _datas.push_back(pData);
        return (int)_datas.size() - 1;
    }

    void Draw(int handle)
    {
        if (handle < 0 || handle >= _datas.size() || _datas[handle] == nullptr)
        {
            return;
        }
        _datas[handle]->nowFrame += _datas[handle]->animSpeed;
        if (_datas[handle]->nowFrame > (float)_datas[handle]->endFrame)
        {
            _datas[handle]->nowFrame = (float)_datas[handle]->startFrame;
        }
        if (_datas[handle]->pFbx)
        {
            _datas[handle]->pFbx->Draw(_datas[handle]->transform, (int)_datas[handle]->nowFrame);
        }
    }

    void Release(int handle)
    {
        if (handle < 0 || handle >= _datas.size() || _datas[handle] == nullptr)
        {
            return;
        }
        bool isExist = false;
        for (int i = 0; i < _datas.size(); i++)
        {
            if (_datas[i] != nullptr && i != handle && _datas[i]->pFbx == _datas[handle]->pFbx)
            {
                isExist = true;
                break;
            }
        }
        if (isExist == false)
        {
            SAFE_DELETE(_datas[handle]->pFbx);
        }
        SAFE_DELETE(_datas[handle]);
    }

    void AllRelease()
    {
        for (int i = 0; i < _datas.size(); i++)
        {
            if (_datas[i] != nullptr)
            {
                Release(i);
            }
        }
        _datas.clear();
    }

    void SetAnimFrame(int handle, int startFrame, int endFrame, float animSpeed)
    {
        if (handle < 0 || handle >= _datas.size() || _datas[handle] == nullptr) return;
        _datas[handle]->SetAnimFrame(startFrame, endFrame, animSpeed);
    }

    int GetAnimFrame(int handle)
    {
        if (handle < 0 || handle >= _datas.size() || _datas[handle] == nullptr) return -1;
        return (int)_datas[handle]->nowFrame;
    }

    XMFLOAT3 GetBonePosition(int handle, std::string boneName)
    {
        if (handle < 0 || handle >= _datas.size() || _datas[handle] == nullptr) return XMFLOAT3(0, 0, 0);
        XMFLOAT3 pos = _datas[handle]->pFbx->GetBonePosition(boneName);
        XMVECTOR vec = XMVector3TransformCoord(XMLoadFloat3(&pos), _datas[handle]->transform.GetWorldMatrix());
        XMStoreFloat3(&pos, vec);
        return pos;
    }

    XMFLOAT3 GetAnimBonePosition(int handle, std::string boneName)
    {
        if (handle < 0 || handle >= _datas.size() || _datas[handle] == nullptr) return XMFLOAT3(0, 0, 0);
        XMFLOAT3 pos = _datas[handle]->pFbx->GetAnimBonePosition(boneName);
        XMVECTOR vec = XMVector3TransformCoord(XMLoadFloat3(&pos), _datas[handle]->transform.GetWorldMatrix());
        XMStoreFloat3(&pos, vec);
        return pos;
    }

    void SetTransform(int handle, Transform& transform)
    {
        if (handle < 0 || handle >= _datas.size() || _datas[handle] == nullptr) return;
        _datas[handle]->transform = transform;
    }

    XMMATRIX GetMatrix(int handle)
    {
        if (handle < 0 || handle >= _datas.size() || _datas[handle] == nullptr) return XMMatrixIdentity();
        return _datas[handle]->transform.GetWorldMatrix();
    }

    void RayCast(int handle, RayCastData* data)
    {
        if (handle < 0 || handle >= _datas.size() || _datas[handle] == nullptr) return;
        XMFLOAT3 target = Transform::Float3Add(data->start, data->dir);
        XMMATRIX matInv = XMMatrixInverse(nullptr, _datas[handle]->transform.GetWorldMatrix());
        XMVECTOR vecStart = XMVector3TransformCoord(XMLoadFloat3(&data->start), matInv);
        XMVECTOR vecTarget = XMVector3TransformCoord(XMLoadFloat3(&target), matInv);
        XMVECTOR vecDir = vecTarget - vecStart;
        XMStoreFloat3(&data->start, vecStart);
        XMStoreFloat3(&data->dir, vecDir);
        _datas[handle]->pFbx->RayCast(data);
    }
}