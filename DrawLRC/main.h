#pragma once

#include "resource.h"
//��
#define MAX_LOADSTRING 100

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);//��������Ϣ������
BOOL WindowCreate(HWND hWnd);//���ڴ������ִ��
BOOL ShowCreateWindow(HINSTANCE hInstance,int nCmdShow);	//����������
BOOL WindowCommand(UINT msg,WPARAM wParam,LPARAM lParam);			//������COMMAND����

//ȫ�ֱ���
HINSTANCE hIns;	//��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];// ����������
HWND hWnd;		//�����ھ��

BackLRC *back_lrc;