// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣
#include "stdafx.h"
#include "DrawLRC.h"
BOOL APIENTRY DllMain(HMODULE hModule,DWORD ul_reason_for_call,LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH://�����DLL��ӳ�䵽�˽��̵ĵ�ַ�ռ�ʱ
		sethInstance(hModule);
		break;
	case DLL_THREAD_ATTACH://һ���߳����ڱ�����
		//MessageBox(NULL,_T("DLL_THREAD_ATTACH"),_T("������"),MB_OK);
		break;
	case DLL_THREAD_DETACH://�߳��ս�
		//MessageBox(NULL,_T("�߳��ս�"),_T("By������"),MB_OK);
		break;
	case DLL_PROCESS_DETACH://���DLL�ӽ��̵ĵ�ַ�ռ��н��ӳ��
		break;
	}
	return TRUE;
}