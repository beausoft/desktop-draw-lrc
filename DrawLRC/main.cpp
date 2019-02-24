#include "stdafx.h"
#include "BackLRC.h"
#include "main.h"

int APIENTRY _tWinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPTSTR lpCmdLine,int nCmdShow)
{
	hIns=hInstance;	//��Ӧ��ʵ�����浽ȫ�ֱ���
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	//��ʼ��ȫ���ַ���
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WINDOWSCPPTEXT, szWindowClass, MAX_LOADSTRING);

	ShowCreateWindow(hInstance,nCmdShow);

	// ����Ϣѭ��:
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, NULL, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int)msg.wParam;
}
//����������
BOOL ShowCreateWindow(HINSTANCE hInstance,int nCmdShow){
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GNORE));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW);//(HBRUSH)GetStockObject(WHITE_BRUSH); //(HBRUSH)(COLOR_BACKGROUND);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_GNORE));
	RegisterClassEx(&wcex);//ע�ᴰ��
	hWnd=CreateWindow(szWindowClass,szTitle,WS_OVERLAPPEDWINDOW^WS_MAXIMIZEBOX^WS_SIZEBOX,CW_USEDEFAULT,0,500,300,NULL,NULL,hInstance,NULL);	//��������
	if(!hWnd)return FALSE;
	ShowWindow(hWnd,nCmdShow);	//��ʾ����
	UpdateWindow(hWnd);	//���´���
	return TRUE;
}

//��������Ϣ������
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	PAINTSTRUCT ps;
	HDC hdc;
	switch (message)
	{
		case WM_COMMAND:
			WindowCommand(message,wParam,lParam);
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_CREATE:
			WindowCreate(hWnd);
			break;
		case WM_CLOSE:
			if(IDYES==MessageBox(hWnd,_T("�Ƿ�ر�?"),_T("��ʾ"),MB_YESNO)){
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
//���ڴ������ִ��
HWND edit;
HWND lblInfo;
BOOL WindowCreate(HWND hWnd){
	edit= CreateWindowEx(NULL,_T("Edit"),_T("Edit"),WS_VISIBLE|WS_CHILD|WS_BORDER,45,5,400,20,hWnd,(HMENU)1000,hIns,NULL);
	HWND button= CreateWindowEx(NULL,_T("Button"),_T("Button"),WS_VISIBLE|WS_CHILD,5,35,100,20,hWnd,(HMENU)1001,hIns,NULL);
	HWND button1= CreateWindowEx(NULL,_T("Button"),_T("Button"),WS_VISIBLE|WS_CHILD,5,140,100,20,hWnd,(HMENU)1003,hIns,NULL);
	HWND button2= CreateWindowEx(NULL,_T("Button"),_T("Button"),WS_VISIBLE|WS_CHILD,105,140,100,20,hWnd,(HMENU)1004,hIns,NULL);
	HWND button3= CreateWindowEx(NULL,_T("Button"),_T("Button"),WS_VISIBLE|WS_CHILD,205,140,100,20,hWnd,(HMENU)1005,hIns,NULL);
	HWND button4= CreateWindowEx(NULL,_T("Button"),_T("Button"),WS_VISIBLE|WS_CHILD,305,140,100,20,hWnd,(HMENU)1006,hIns,NULL);
	HWND button5= CreateWindowEx(NULL,_T("Button"),_T("Button"),WS_VISIBLE|WS_CHILD,5,160,100,20,hWnd,(HMENU)1007,hIns,NULL);
	HWND button6= CreateWindowEx(NULL,_T("Button"),_T("Button"),WS_VISIBLE|WS_CHILD,105,160,100,20,hWnd,(HMENU)1008,hIns,NULL);
	HWND button7= CreateWindowEx(NULL,_T("Button"),_T("Button"),WS_VISIBLE|WS_CHILD,205,160,100,20,hWnd,(HMENU)1009,hIns,NULL);
	lblInfo=CreateWindowEx(NULL,_T("Static"),_T("Static"),WS_VISIBLE|WS_CHILD,8,5,35,20,hWnd,(HMENU)1002,hIns,NULL);
	SetWindowText(button,_T("������"));
	SetWindowText(button1,_T("�ı�1"));
	SetWindowText(button2,_T("�ı�2"));
	SetWindowText(button3,_T("��ɫ1"));
	SetWindowText(button4,_T("��ɫ2"));
	SetWindowText(button5,_T("����ok"));
	SetWindowText(button6,_T("����|ȡ��"));
	SetWindowText(button7,_T("��ɫ3"));
	
	SetWindowText(lblInfo,_T("����:"));
	SetWindowText(edit,_T("��Ϊ�ǲ�����Ŀ��û���жϣ����������룬���򱨴�"));
	return TRUE;
}
//������COMMAND����
HMODULE lrc;
typedef void(*CreateLRC)();
typedef void(*InitGDI)();
typedef void(*ReleaseDrawLRC)();
typedef void(*DrawLRC_SHOW_TEXT)(WCHAR *text);
typedef void(*SetLRCColor)(int lrc_color);
typedef void(*SetDrawLRCTime)(LONG time);
typedef void(*LockingLRC)();

DrawLRC_SHOW_TEXT draw_text;
SetLRCColor setcolor;
SetDrawLRCTime settime;
LockingLRC lockingLRC;
BOOL WindowCommand(UINT msg,WPARAM wParam,LPARAM lParam){
	switch (wParam)
	{
		case 1001:
			lrc=LoadLibrary(_T("DrawLRC_DLL.dll"));
			InitGDI in;
			in=(InitGDI)GetProcAddress(lrc,"InitGDI");
			in();
			CreateLRC mb ;
			mb= (CreateLRC)GetProcAddress(lrc,"CreateLRC");
			mb();
			draw_text=(DrawLRC_SHOW_TEXT)GetProcAddress(lrc,"DrawLRC_SHOW_TEXT");
			setcolor=(SetLRCColor)GetProcAddress(lrc,"SetLRCColor");
			settime=(SetDrawLRCTime)GetProcAddress(lrc,"SetDrawLRCTime");
			lockingLRC=(LockingLRC)GetProcAddress(lrc,"LockingLRC");
			//draw_text(_T("����0"));
			break;
		case 1003:
			//ReleaseDrawLRC re;
			//re=(ReleaseDrawLRC)GetProcAddress(lrc,"ReleaseDrawLRC");
			//re();
			//FreeLibrary(lrc);
			draw_text(_T("�����ǿ�ʼ�ú����� ������û���� �����Ǻܿ�ϧ"));
			break;
		case 1004:
			draw_text(_T("Ҳ��Ӧ�ö������� Ӧ�����������������"));
			break;
		case 1005:
			setcolor(2);
			break;
		case 1006:
			setcolor(4);
			break;
		case 1007:
			settime(5);
			break;
		case 1008:
			lockingLRC();
			break;
		case 1009:
			setcolor(1);
			break;
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return TRUE;
}