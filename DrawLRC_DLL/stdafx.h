#pragma once
#include "targetver.h"

//#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�:
#include <windows.h>

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

//GDI��
#include <GdiPlus.h>
#pragma comment(lib,"GdiPlus.lib")
using namespace Gdiplus;
using namespace DllExports;

//string�ַ���
#include <string>
using  std::string;
using  std::wstring;

//�ܹ�ʹ��Visual C�����е�һЩ����COM֧���ࡣ�������ͷ�ļ���gdiplus.h�����ж����Ϳ������ˡ�
//#include <comdef.h>