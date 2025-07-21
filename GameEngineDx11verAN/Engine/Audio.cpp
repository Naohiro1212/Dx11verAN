#include "Audio.h"
#include <vector>
#include <xaudio2.h>

#define SAFE_DELETE_ARRAY(p) if(p){delete[] p; p = nullptr;}
#define READ_AND_CHECK(hFile, ptr, size, dwBytes) \
    (ReadFile((hFile), (ptr), (DWORD)(size), &(dwBytes), NULL) && (dwBytes) == (size))

namespace Audio
{
    // XAudio本体
    IXAudio2* pXAudio = nullptr;

    // マスターボイス
    IXAudio2MasteringVoice* pMasteringVoice = nullptr;

    // ファイルごとに必要な情報
    struct AudioData
    {
        // サウンド情報
        XAUDIO2_BUFFER buf = {};

        // ソースボイス
        IXAudio2SourceVoice** pSourceVoice = nullptr;

        // 同時再生最大数
        int svNum;

        // ファイル名
        std::wstring fileName;
    };
    std::vector<AudioData> audioDatas;
}

// 初期化
void Audio::Initialize()
{
    HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hr)) {
        // エラー：COM初期化失敗
        return;
    }

    hr = XAudio2Create(&pXAudio);
    if (FAILED(hr)) {
        // エラー：XAudio2生成失敗
        CoUninitialize();
        return;
    }

    hr = pXAudio->CreateMasteringVoice(&pMasteringVoice);
    if (FAILED(hr)) {
        // エラー：マスターボイス生成失敗
        pXAudio->Release();
        CoUninitialize();
        return;
    }
}

// サウンドファイル(.wav)をロード
int Audio::Load(std::wstring fileName, bool isLoop, int svNum)
{
    // すでに同じファイルを使ってないかチェック
    for (int i = 0; i < audioDatas.size(); i++)
    {
        if (audioDatas[i].fileName == fileName)
        {
            return i;
        }
    }

    // チャンク構造体
    struct Chunk
    {
        char id[5] = ""; // ID
        unsigned int size = 0; // サイズ
    };

    // ファイルを開く
    HANDLE hFile;
    hFile = CreateFileW(fileName.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    DWORD dwBytes = 0;

    Chunk riffChunk = { 0 };
    if (!READ_AND_CHECK(hFile, &riffChunk.id, sizeof(riffChunk.id), dwBytes)) {
        CloseHandle(hFile);
        return -1;
    }
    if (!READ_AND_CHECK(hFile, &riffChunk.size, sizeof(riffChunk.size), dwBytes)) {
        CloseHandle(hFile);
        return -1;
    }

    char wave[4] = "";
    if (!READ_AND_CHECK(hFile, &wave, sizeof(wave), dwBytes)) {
        CloseHandle(hFile);
        return -1;
    }

    // ---- formatチャンク
    Chunk formatChunk = { 0 };
    do {
        if (!READ_AND_CHECK(hFile, &formatChunk.id, sizeof(formatChunk.id), dwBytes)) {
            CloseHandle(hFile);
            return -1;
        }
        // 目的のチャンクが見つかるまで繰り返し
    } while (formatChunk.id[0] != 'f');

    if (!READ_AND_CHECK(hFile, &formatChunk.size, sizeof(formatChunk.size), dwBytes)) {
        CloseHandle(hFile);
        return -1;
    }

    // ---- WAVEFORMATEX
    WAVEFORMATEX fmt;
    if (!READ_AND_CHECK(hFile, &fmt.wFormatTag, sizeof(fmt.wFormatTag), dwBytes)) { CloseHandle(hFile); return -1; }
    if (!READ_AND_CHECK(hFile, &fmt.nChannels, sizeof(fmt.nChannels), dwBytes)) { CloseHandle(hFile); return -1; }
    if (!READ_AND_CHECK(hFile, &fmt.nSamplesPerSec, sizeof(fmt.nSamplesPerSec), dwBytes)) { CloseHandle(hFile); return -1; }
    if (!READ_AND_CHECK(hFile, &fmt.nAvgBytesPerSec, sizeof(fmt.nAvgBytesPerSec), dwBytes)) { CloseHandle(hFile); return -1; }
    if (!READ_AND_CHECK(hFile, &fmt.nBlockAlign, sizeof(fmt.nBlockAlign), dwBytes)) { CloseHandle(hFile); return -1; }
    if (!READ_AND_CHECK(hFile, &fmt.wBitsPerSample, sizeof(fmt.wBitsPerSample), dwBytes)) { CloseHandle(hFile); return -1; }

    // ---- dataチャンク
    Chunk data = { 0 };
    while (true) {
        if (!READ_AND_CHECK(hFile, &data.id, sizeof(data.id), dwBytes)) {
            CloseHandle(hFile);
            return -1;
        }
        if (strcmp(data.id, "data") == 0) break;

        // それ以外の情報はスキップ
        if (!READ_AND_CHECK(hFile, &data.size, sizeof(data.size), dwBytes)) {
            CloseHandle(hFile);
            return -1;
        }
        if (data.size > 32 * 1024 * 1024) { // 例: 32MB以上で異常終了
            CloseHandle(hFile);
            return -1;
        }
        char* pBuffer = new(std::nothrow) char[data.size];
        if (!pBuffer) {
            CloseHandle(hFile);
            return -1;
        }
        if (!READ_AND_CHECK(hFile, pBuffer, data.size, dwBytes)) {
            delete[] pBuffer;
            CloseHandle(hFile);
            return -1;
        }
        delete[] pBuffer;
    }

    // データチャンクのサイズ読み取り
    if (!READ_AND_CHECK(hFile, &data.size, sizeof(data.size), dwBytes)) {
        CloseHandle(hFile);
        return -1;
    }
    if (data.size > 32 * 1024 * 1024) { // サイズ上限チェック例
        CloseHandle(hFile);
        return -1;
    }

    // 波形データ読み込み
    char* pBuffer = new(std::nothrow) char[data.size];
    if (!pBuffer) {
        CloseHandle(hFile);
        return -1;
    }
    if (!READ_AND_CHECK(hFile, pBuffer, data.size, dwBytes)) {
        delete[] pBuffer;
        CloseHandle(hFile);
        return -1;
    }

    CloseHandle(hFile);

    AudioData ad;
    ad.fileName = fileName;
    ad.buf.pAudioData = (BYTE*)pBuffer;
    ad.buf.Flags = XAUDIO2_END_OF_STREAM;

    if (isLoop) ad.buf.LoopCount = XAUDIO2_LOOP_INFINITE;
    ad.buf.AudioBytes = data.size;

    ad.pSourceVoice = new IXAudio2SourceVoice * [svNum];

    for (int i = 0; i < svNum; i++)
    {
        pXAudio->CreateSourceVoice(&ad.pSourceVoice[i], &fmt);
    }
    ad.svNum = svNum;
    audioDatas.push_back(ad);

    return (int)audioDatas.size() - 1;
}

// 再生
void Audio::Play(int ID)
{
    for (int i = 0; i < audioDatas[ID].svNum; i++)
    {
        XAUDIO2_VOICE_STATE state;
        audioDatas[ID].pSourceVoice[i]->GetState(&state);
        
        if (state.BuffersQueued == 0)
        {
            audioDatas[ID].pSourceVoice[i]->SubmitSourceBuffer(&audioDatas[ID].buf);
            audioDatas[ID].pSourceVoice[i]->Start();
            break;
        }
    }
}

void Audio::Stop(int ID)
{
    for (int i = 0; i < audioDatas[ID].svNum; i++)
    {
        audioDatas[ID].pSourceVoice[i]->Stop();
        audioDatas[ID].pSourceVoice[i]->FlushSourceBuffers();
    }
}

// シーンごとの解放
void Audio::Release()
{
    for (int i = 0; i < audioDatas.size(); i++)
    {
        for (int j = 0; j < audioDatas[i].svNum; j++)
        {
            audioDatas[i].pSourceVoice[j]->DestroyVoice();
        }
        SAFE_DELETE_ARRAY(audioDatas[i].buf.pAudioData);
    }
    audioDatas.clear();
}

// 本体の解放
void Audio::AllRelease()
{
    CoUninitialize();
    if (pMasteringVoice)
    {
        pMasteringVoice->DestroyVoice();
    }
    pXAudio->Release();
}
