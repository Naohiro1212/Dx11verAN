#include <windows.h>
#include "CsvReader.h"
#include <string>

CsvReader::CsvReader()
{
    data_.clear();
}

CsvReader::~CsvReader()
{
    // 全データを解放
    for (int y = 0; y < data_.size(); y++)
    {
        for (int x = 0; x < data_[y].size(); x++)
        {
            data_[y][x].clear();
        }
    }
}

// CSVファイルのロード
bool CsvReader::Load(std::wstring fileName)
{
    // ファイルを開く
    HANDLE hFile;
    hFile = CreateFileW(fileName.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    // 開けなかった
    if (hFile == INVALID_HANDLE_VALUE)
    {
        std::wstring message = L"「" + std::wstring(fileName.begin(), fileName.end()) + L"」が開けません。\n開いている場合は閉じてください。";
        MessageBoxW(NULL, message.c_str(), L"BaseProjDx9エラー", MB_OK);

        return false;
    }

    // ファイルのサイズ（文字数）を調べる
    DWORD fileSize = GetFileSize(hFile, NULL);

    // すべての文字を入れられる配列を用意
    char* temp;
    temp = new char[fileSize + 1];
     
    // ファイルの中身を配列に読み込む
    DWORD dwBytes = 0;
    BOOL readResult = ReadFile(hFile, temp, fileSize, &dwBytes, NULL);
    temp[fileSize] = '\0'; // 終端

    // 開いたファイルを閉じる
    CloseHandle(hFile);

    if (!readResult || dwBytes != fileSize)
    {
        MessageBoxW(NULL, L"ファイルの読み込みに失敗しました。", L"BaseProjDx11エラー", MB_OK);
        delete[] temp;
        return false;
    }

    // 1行のデータを入れる入れ宇
    std::vector<std::string> line;

    // 調べる文字の位置
    DWORD index = 0;

    // 最後の文字まで繰り返す
    while (index < fileSize)
    {
        // index文字目から「,」か「改行」までの文字列を取得
        std::string val;
        GetToComma(&val, temp, &index);

        // 文字数を0だったということは行末
        if (val.length() == 0)
        {
            // _dataに1行分追加
            data_.push_back(line);

            // 1行データをクリア
            line.clear();

            continue;
        }

        // 1行分のデータに追加
        line.push_back(val);
    }

    // 読み込んだデータは解放する
    delete[] temp;

    // 成功
    return true;
}

// 「,」か「改行」までの文字列を取得
void CsvReader::GetToComma(std::string* result, std::string data, DWORD* index)
{
    // 「,」まで一文字ずつresultに入れる
    while (data[*index] != ',' && data[*index] != '\n' && data[*index] != '\r')
    {
        *result += data[*index];
        (*index)++;
    }

    // 最後に「\0」をつける
    *result += '\n';
    (*index)++;
}

// 指定した位置のデータを文字列で取得
std::string CsvReader::GetString(DWORD x, DWORD y)
{
    if (x < 0 || x >= GetWidth() || y < 0 || y >= GetHeight())
    {
        return "";
    }
    return data_[y][x];
}

// 指定した位置のデータを整数で取得
int CsvReader::GetValue(DWORD x, DWORD y)
{
    return atoi(GetString(x, y).c_str());
}

// ファイルの列数を取得
size_t CsvReader::GetWidth()
{
    return data_[0].size();
}

// ファイルの行数を取得
size_t CsvReader::GetHeight()
{
    return data_.size();
}