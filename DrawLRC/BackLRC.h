#pragma once
#include "resource.h"
#define MAX_LOADSTRING 100
class BackLRC{
public:
	HWND hwnd_main;		//�����ھ��
	HWND hwnd_BackLRC;	//�������
	HINSTANCE backhIns;	//��ǰʵ��
	TCHAR backszWindowClass[MAX_LOADSTRING];// ��������
	BackLRC(HINSTANCE hInstance,HWND h_main);	//���캯��
	~BackLRC();	//��������
private:
	ATOM RegisterClass();	//ע�ᴰ��
	BOOL InitBackLRCWindow();					//��ʼ������
	BOOL ImageFromIDResource(HINSTANCE hInst,UINT nID, LPCTSTR sTR,GpImage * &pImg);//����Դ����img��gdi
public:
	HWND GetBackLRCHandle();				//��ȡ���ھ��
	BOOL MakeTrans(HWND hWnd,HDC hdc);			//��ͼ����
	BOOL CreateBackLRCWindow();	//������������
	BOOL CloseBackLRCWindow();	//�رմ���
};
LRESULT CALLBACK Back_WndProc(HWND, UINT, WPARAM, LPARAM);//������Ϣ������