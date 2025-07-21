#pragma once
#include "Direct3D.h"

// ˆÀ‘S‚Éƒƒ‚ƒŠ‚ð‰ð•ú‚·‚é‚½‚ß‚Ìƒ}ƒNƒ
#define SAFE_DELETE(p) {if ((p)!=nullptr){delete(p); (p)=nullptr;}}
#define SAFE_DELETE_ARRAY(p) {if((p)!=nullptr) {delete[] (p); (p)=nullptr;}}
#define SAFE_RELEASE(p) {if((p)!=nullptr){p->Release();(p)=nullptr;}}