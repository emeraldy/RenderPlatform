#pragma once

//-----------------------------------------------------------------
// Macros Definition
//-----------------------------------------------------------------
#define SAFE_DELETE(p) { if(p) { delete p; p = nullptr; } }
#define SAFE_DELETEARRAY(p) { if(p) { delete[] p; p = nullptr; } }
#define RELEASECOM(x) { if(x) { x->Release(); x = nullptr; } }