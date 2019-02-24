#include "stdafx.h"
#include "DrawLRC.h"
#include "resource.h"

ULONG token;	//GDI+���
HINSTANCE hInstance;	//Ӧ�ó���ʵ�����
HWND back_hwnd;		//�������ھ��
HWND show_hwnd;		//��ʾ��ʴ��ھ��
HWND lrc_hwnd;		//����ok���ھ��
BOOL isMousePrees;	//����Ƿ���
HDC mdc;
HBITMAP mainBitmap;
HGDIOBJ oldBitmap;
BLENDFUNCTION blendFunc32bpp;
POINT mouse_xy;
//����
GdiFont GFont;
GpFontFamily *fontFam=NULL;
GpStringFormat *strFormat=NULL;
GpPath *strPath=NULL;
Rect rclayout;
//��ʾ���ڱ�������
HDC mdc_show=NULL;
HBITMAP mainBitmap_show=NULL;
HGDIOBJ oldBitmap_show=NULL;
GpFontFamily *fontFam_show=NULL;
GpStringFormat *strFormat_show=NULL;
GpPath *strPath_show=NULL;
GpLineGradient *Brush_show=NULL;
GpPen *pen_show=NULL;
GpGraphics *graphics_show=NULL;
HDC dc_show=NULL;	//��ʾ���ڵ�DC
//����ok���ڱ���
HDC mdc_lrc;
HGDIOBJ oldBitmap_lrc=NULL;
HBITMAP mainBitmap_lrc=NULL;
GpFontFamily *fontFam_lrc=NULL;
GpStringFormat *strFormat_lrc=NULL;
GpPath *strPath_lrc=NULL;
GpLineGradient *Brush_lrc=NULL;
GpPen *pen_lrc=NULL;
GpGraphics *graphics_lrc=NULL;
Rect rclayout_lrc;
HDC dc_lrc=NULL;	//����OK���ڵ�DC

//��ɫ����
LRC_COLOR drawlrc_color;

//����ı�
WCHAR *LRCText=NULL;
LONG LrcTime=0;	//���ʱ��
INT str_count=0;	//�ַ�����
INT str_width=0;	//�ַ����

BOOL isInitLRC=FALSE;
BOOL isLocking=FALSE;

//�����ⲿ����
extern "C" _declspec(dllexport) void CreateLRC();
extern "C" _declspec(dllexport) void About();
extern "C" _declspec(dllexport) void InitGDI();
extern "C" _declspec(dllexport) BOOL ReleaseDrawLRC();
extern "C" _declspec(dllexport) void DrawLRC_SHOW_TEXT(WCHAR *text);
extern "C" _declspec(dllexport) void SetLRCColor(LRC_COLOR lrc_color);
extern "C" _declspec(dllexport) void SetDrawLRCTime(LONG time);
extern "C" _declspec(dllexport) void LockingLRC();
//ʵ���ⲿ����
void CreateLRC(){
	if(!isInitLRC){
		isInitLRC=TRUE;	//���ó�ʼ�����
		//��ʼ������
		GFont.FontBold=FALSE;
		GFont.FontName=_T("����");
		GFont.FontSize=41;
		GFont.FontWidth=0;
		GFont.LineColor=0;
		GFont.OkColor1=0;
		GFont.OKColor2=0;
		drawlrc_color=LRC_COLOR_White;
		LRC_BACK_COLOR show_color=GetShowColor(drawlrc_color);
		GFont.ForeColor1=show_color.LRC_BACK_COLOR1;
		GFont.ForeColor2=show_color.LRC_BACK_COLOR2;
		CreateLrcWindow();
		CreateShowWindow();
		CreateBackWindow();

		RECT show_rect;
		GetWindowRect(show_hwnd,&show_rect);
		RECT back_rect;
		GetWindowRect(back_hwnd,&back_rect);
		MoveWindow(back_hwnd,0,show_rect.top-(130-GFont.FontSize)/3,back_rect.right-back_rect.left,back_rect.bottom-back_rect.top,TRUE);

		SetZOrder();
	}
}
void About(){
	MessageBox(NULL,_T("�������ɺ�����������QQ:546168949��\r\n�����򲻿�������ҵ��;���������һ�о��ױ��˸Ų�����"),_T("����...By:������"),MB_OK);
}
void InitGDI(){
	GdiplusStartupInput GpInput;
	GpInput.GdiplusVersion=1;
	if(GdiplusStartup(&token,&GpInput,NULL)!=0){
		MessageBox(NULL,_T("Fehler bem laden von GDI+!"),_T("��ʾ"),MB_OK);
	}
}
BOOL ReleaseDrawLRC(){
	GdiplusShutdown(token);//�ͷ�GDI+
	//�ͷű�������
	SelectObject(mdc,oldBitmap);
	DeleteObject(mainBitmap);
	DeleteObject(oldBitmap);
	DeleteDC(mdc);
	//DestroyWindow(back_hwnd);	//���ٴ���
	//�ͷ���ʾ����
	SelectObject(mdc_show,oldBitmap_show);
	DeleteObject(mainBitmap_show);
	DeleteObject(oldBitmap_show);
	DeleteDC(mdc_show);
	GdipDeleteFontFamily(fontFam_show);
	GdipDeleteStringFormat(strFormat_show);
	GdipDeletePath(strPath_show);
	GdipDeleteBrush(Brush_show);
	GdipDeletePen(pen_show);
	GdipDeleteGraphics(graphics_show);
	return TRUE;
}
void DrawLRC_SHOW_TEXT(WCHAR *text){
	ShowWindow(lrc_hwnd,SW_HIDE);	//���ش���
	KillTimer(lrc_hwnd,TIMELRC);
	//RECT lrc_rect_tmp;
	//GetWindowRect(lrc_hwnd,&lrc_rect_tmp);
	//MoveWindow(lrc_hwnd,lrc_rect_tmp.left,lrc_rect_tmp.top,0,lrc_rect_tmp.bottom-lrc_rect_tmp.top,TRUE);
	Show_DrawText(text,dc_show);
	lrc_DrawText(text,dc_lrc);
	LRCText=text;
}
void SetLRCColor(LRC_COLOR lrc_color){
	drawlrc_color=lrc_color;
	LRC_BACK_COLOR show_color=GetShowColor(lrc_color);
	GFont.ForeColor1=show_color.LRC_BACK_COLOR1;
	GFont.ForeColor2=show_color.LRC_BACK_COLOR2;
	//��������ɫ��Ҫ�ӻ�
	Show_DrawText(LRCText,dc_show);
	//RECT lrc_rect_tmp;
	//GetWindowRect(lrc_hwnd,&lrc_rect_tmp);
	//INT ScreenWidth=GetSystemMetrics(SM_CXSCREEN);	//��ȡ��Ļ���
	//INT lrc_widtmp=lrc_rect_tmp.right-lrc_rect_tmp.left;
	//MoveWindow(lrc_hwnd,lrc_rect_tmp.left,lrc_rect_tmp.top,ScreenWidth,lrc_rect_tmp.bottom-lrc_rect_tmp.top,TRUE);
	lrc_DrawText(LRCText,dc_lrc);
	//MoveWindow(lrc_hwnd,lrc_rect_tmp.left,lrc_rect_tmp.top,lrc_widtmp,lrc_rect_tmp.bottom-lrc_rect_tmp.top,TRUE);
}
void SetDrawLRCTime(LONG time){
	if(LRCText!=NULL){
		KillTimer(lrc_hwnd,TIMELRC);
		LrcTime=time;

		str_count=wcslen(LRCText);
		str_width=str_count*GFont.FontSize;
		
		RECT lrc_rect_tmp;
		GetWindowRect(lrc_hwnd,&lrc_rect_tmp);
		INT ScreenWidth=GetSystemMetrics(SM_CXSCREEN);	//��ȡ��Ļ���
		INT Width=(ScreenWidth-str_width)/2;		
		MoveWindow(lrc_hwnd,lrc_rect_tmp.left,lrc_rect_tmp.top,Width,lrc_rect_tmp.bottom-lrc_rect_tmp.top,TRUE);
		//SetWindowPos(lrc_hwnd,HWND_TOPMOST,NULL,NULL,Width,lrc_rect_tmp.bottom-lrc_rect_tmp.top,SWP_NOMOVE);
		//UpdateWindow(lrc_hwnd);
		ShowWindow(lrc_hwnd,SW_SHOWNOACTIVATE);	//��ʾ���ڲ������
		SetTimer(lrc_hwnd,TIMELRC,10,TimerProc);	//���ö�ʱ��
	}
}
void LockingLRC(){
	LONG lrc_ret=GetWindowLong(lrc_hwnd,GWL_EXSTYLE);
	LONG show_ret=GetWindowLong(show_hwnd,GWL_EXSTYLE);

	if(!isLocking){
		lrc_ret=lrc_ret|WS_EX_LAYERED|WS_EX_TRANSPARENT;
		SetWindowLong(lrc_hwnd,GWL_EXSTYLE,lrc_ret);
		show_ret=show_ret|WS_EX_LAYERED|WS_EX_TRANSPARENT;
		SetWindowLong(show_hwnd,GWL_EXSTYLE,show_ret);
		isLocking=TRUE;
	}else{
		lrc_ret=lrc_ret^WS_EX_TRANSPARENT;
		SetWindowLong(lrc_hwnd,GWL_EXSTYLE,lrc_ret);
		show_ret=show_ret^WS_EX_TRANSPARENT;
		SetWindowLong(show_hwnd,GWL_EXSTYLE,show_ret);
		isLocking=FALSE;
	}
}

BOOL sethInstance(HINSTANCE hIns){
	hInstance=hIns;
	return TRUE;
}
HWND getBACK_HWND(){
	return back_hwnd;
}
//������������
BOOL CreateBackWindow(){
	RECT rect;
	int ScreenWidth=GetSystemMetrics(SM_CXSCREEN);	//��ȡ��Ļ���
	int Rwheight=GetSystemMetrics(SM_CYFULLSCREEN);	//��ȡ�ɼ���Ļ�߶�(������������)
	rect.left=0;
	rect.right=ScreenWidth;
	rect.top=Rwheight-103;
	rect.bottom=Rwheight+103;
	CreateWindowUniversal(back_hwnd,_T("lrc_back"),Back_WndProc,rect);
	HDC hdc_back;
	hdc_back=GetWindowDC(back_hwnd);
	MakeTrans(back_hwnd,hdc_back);	//���Ʊ�������
	//ShowWindow(back_hwnd,SW_SHOW);	//֮ǰ���������صģ���ֹ��˸
	return TRUE;
}
//������ʾ��ʴ���
BOOL CreateShowWindow(){
	RECT rect;
	int ScreenWidth=GetSystemMetrics(SM_CXSCREEN);	//��ȡ��Ļ���
	//int ScrrenHeight=GetSystemMetrics(SM_CYSCREEN);	//��ȡ��Ļ�߶�
	int Rwheight=GetSystemMetrics(SM_CYFULLSCREEN);	//��ȡ�ɼ���Ļ�߶�(������������)
	rect.left=0;
	rect.right=ScreenWidth;
	rect.top=Rwheight-103;
	rect.bottom=Rwheight+103;
	CreateWindowUniversal(show_hwnd,_T("lrc_show"),Show_WndProc,rect);
	SetWindowPos(show_hwnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	dc_show=GetWindowDC(show_hwnd);
	Show_DrawText(_T("�ڿ͹��̣�������"),dc_show);
	ShowWindow(show_hwnd, SW_SHOW);
	return TRUE;
}
//��������ok����
BOOL CreateLrcWindow(){
	RECT rect;
	int ScreenWidth=GetSystemMetrics(SM_CXSCREEN);	//��ȡ��Ļ���
	//int ScrrenHeight=GetSystemMetrics(SM_CYSCREEN);	//��ȡ��Ļ�߶�
	int Rwheight=GetSystemMetrics(SM_CYFULLSCREEN);	//��ȡ�ɼ���Ļ�߶�(������������)
	rect.left=0;
	rect.right=ScreenWidth;
	rect.top=Rwheight-103;
	rect.bottom=Rwheight+103;
	CreateWindowUniversal(lrc_hwnd,_T("lrc_lrc"),Lrc_WndProc,rect);
	SetWindowPos(lrc_hwnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	dc_lrc=GetWindowDC(lrc_hwnd);
	lrc_DrawText(_T("�ڿ͹��̣�������"),dc_lrc);
	//ShowWindow(lrc_hwnd, SW_SHOW);
	return TRUE;
}
//����������Ϣ������
LRESULT CALLBACK Back_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	PAINTSTRUCT ps;
	HDC hdc;
	switch (message)
	{
		case WM_COMMAND:
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			//MakeTrans(hWnd,hdc);	//���Ʊ���
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_CREATE:
			SetTimer(hWnd,TIMEWINDMONITOR,100,TimerProc);	//���ö�ʱ��
			break;
		case WM_LBUTTONDOWN:	//��갴��
			SetCapture(hWnd);	//��ռ�����Ϣ
			mouse_xy.y=HIWORD(lParam);
			isMousePrees=TRUE;
			break;
		case WM_LBUTTONUP:		//����ɿ�
			ReleaseCapture();	//�ͷŶ�ռ�����Ϣ
			isMousePrees=FALSE;
			break;
		case WM_MOUSEMOVE:		//����ƶ�
			if(isMousePrees){
				RECT win_re;	//���ڴ�С����
				GetWindowRect(hWnd,&win_re);	//��ȡ���ڴ�С
				int MOUSE_Y=HIWORD(lParam);
				if(isMouseCoordinate(0,MOUSE_Y)){
				int Y=win_re.top+(MOUSE_Y-mouse_xy.y);
				MoveWindow(hWnd,0,Y,win_re.right-win_re.left,win_re.bottom-win_re.top,TRUE);	//�ƶ�����
				//�ƶ���ʾ��ʴ��ںͿ���ok����
				RECT show_rect;
				GetWindowRect(show_hwnd,&show_rect);
				MoveWindow(show_hwnd,show_rect.left,Y+(130-GFont.FontSize)/3,show_rect.right-show_rect.left,show_rect.bottom-show_rect.top,TRUE);
				RECT lrc_rect;
				GetWindowRect(lrc_hwnd,&lrc_rect);
				MoveWindow(lrc_hwnd,show_rect.left,Y+(130-GFont.FontSize)/3,lrc_rect.right-lrc_rect.left,lrc_rect.bottom-lrc_rect.top,TRUE);
				}
			}
			break;
		case WM_KILLFOCUS:	//����ʧȥ����
			isMousePrees=FALSE;
			break;
		case WM_ACTIVATE:	//���ڼ���
			//return MA_NOACTIVATEANDEAT;
			break;
		case WM_MOUSEACTIVATE:
			//return MA_NOACTIVATEANDEAT;
			break;
		case WM_MOUSEHOVER:	//����뿪����
			isMousePrees=FALSE;
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
//��ʾ��ʴ�����
LRESULT CALLBACK Show_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	PAINTSTRUCT ps;
	HDC hdc;
	TRACKMOUSEEVENT tme;
	switch (message)
	{
		case WM_CREATE:
			break;
		case WM_COMMAND:
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		//case WM_MOUSEMOVE:
		//	tme.cbSize=sizeof(TRACKMOUSEEVENT);
		//	tme.dwFlags=TME_HOVER|TME_LEAVE;	//ע�����HOVER��LEAVE��Ϣ
		//	tme.dwHoverTime=100;				//�¼�����ʱ��
		//	tme.hwndTrack=hWnd;					//��ǰ���
		//	TrackMouseEvent(&tme);				//ע��
		//	break;
		case WM_LBUTTONDOWN:	//��갴��
			/*ReleaseCapture();
			SendMessage(hWnd,WM_SYSCOMMAND,SC_MOVE|HTCAPTION,0);*/
			SetCapture(hWnd);	//��ռ���
			mouse_xy.y=HIWORD(lParam);
			mouse_xy.x=LOWORD(lParam);
			isMousePrees=TRUE;
			break;
		case WM_LBUTTONUP:		//����ɿ�
			isMousePrees=FALSE;
			ReleaseCapture();	//�ͷŶ�ռ���
			break;
		case WM_MOUSEMOVE:		//����ƶ�
			if(isMousePrees){
				RECT win_re;	//���ڴ�С����
				GetWindowRect(hWnd,&win_re);	//��ȡ���ڴ�С
				int MOUSE_Y=HIWORD(lParam);
				int MOUSE_X=LOWORD(lParam);
				if(isMouseCoordinate(MOUSE_X,MOUSE_Y)){
				int Y=win_re.top+(MOUSE_Y-mouse_xy.y);
				int X=win_re.left+(MOUSE_X-mouse_xy.x);
				MoveWindow(hWnd,X,Y,win_re.right-win_re.left,win_re.bottom-win_re.top,TRUE);	//�ƶ�����
				//�ƶ��������ں�lrc����
				RECT back_rect;
				GetWindowRect(back_hwnd,&back_rect);
				MoveWindow(back_hwnd,0,Y-(130-GFont.FontSize)/3,back_rect.right-back_rect.left,back_rect.bottom-back_rect.top,TRUE);
				RECT lrc_rect;
				GetWindowRect(lrc_hwnd,&lrc_rect);
				MoveWindow(lrc_hwnd,X,Y,lrc_rect.right-lrc_rect.left,lrc_rect.bottom-lrc_rect.top,TRUE);
				}
			}
			break;
		//case WM_MOUSEHOVER:
		//	if(!isBackShow){
		//		ShowWindow(back_hwnd,SW_SHOW);
		//		isBackShow=TRUE;
		//	}
		//	break;
		//case WM_MOUSELEAVE:
		//	/*if(isBackShow){
		//		ShowWindow(back_hwnd,SW_HIDE);
		//		isBackShow=FALSE;
		//	}*/
		//	break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
//����ok������
LRESULT CALLBACK Lrc_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	PAINTSTRUCT ps;
	HDC hdc;
	switch (message)
	{
		case WM_COMMAND:
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_CREATE:
			break;
		case WM_LBUTTONDOWN:	//��갴��
			/*ReleaseCapture();
			SendMessage(hWnd,WM_SYSCOMMAND,SC_MOVE|HTCAPTION,0);*/
			SetCapture(hWnd);	//��ռ���
			mouse_xy.y=HIWORD(lParam);
			mouse_xy.x=LOWORD(lParam);
			isMousePrees=TRUE;
			break;
		case WM_LBUTTONUP:		//����ɿ�
			isMousePrees=FALSE;
			ReleaseCapture();	//�ͷŶ�ռ���
			break;
		case WM_MOUSEMOVE:		//����ƶ�
			if(isMousePrees){
				RECT win_re;	//���ڴ�С����
				GetWindowRect(hWnd,&win_re);	//��ȡ���ڴ�С
				int MOUSE_Y=HIWORD(lParam);
				int MOUSE_X=LOWORD(lParam);
				if(isMouseCoordinate(MOUSE_X,MOUSE_Y)){
				int Y=win_re.top+(MOUSE_Y-mouse_xy.y);
				int X=win_re.left+(MOUSE_X-mouse_xy.x);
				MoveWindow(hWnd,X,Y,win_re.right-win_re.left,win_re.bottom-win_re.top,TRUE);	//�ƶ�����
				//�ƶ���ʾ��ʴ��ںͱ�������
				RECT show_rect;
				GetWindowRect(show_hwnd,&show_rect);
				MoveWindow(show_hwnd,X,Y,show_rect.right-show_rect.left,show_rect.bottom-show_rect.top,TRUE);
				RECT back_rect;
				GetWindowRect(back_hwnd,&back_rect);
				MoveWindow(back_hwnd,0,Y-(130-GFont.FontSize)/3,back_rect.right-back_rect.left,back_rect.bottom-back_rect.top,TRUE);
				}
			}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
//����OK���Ʒ���
BOOL lrc_DrawText(const WCHAR* text,HDC hdc){
	SelectObject(mdc_lrc,oldBitmap_lrc);
	DeleteObject(mainBitmap_lrc);
	DeleteObject(oldBitmap_lrc);
	DeleteDC(mdc_lrc);
	GdipDeleteFontFamily(fontFam_lrc);
	GdipDeleteStringFormat(strFormat_lrc);
	GdipDeletePath(strPath_lrc);
	GdipDeleteBrush(Brush_lrc);
	GdipDeletePen(pen_lrc);
	GdipDeleteGraphics(graphics_lrc);
	//�ͷ���Դ
	BITMAPINFO tempBI;
	BLENDFUNCTION tempBlend;
	UINT lngHeight=0,lngWidth=0;
	LONG curWinLong=0;
	SIZE winSize;
	POINT srcPoint;
	//��ȡ����λ��
	RECT rect;
	GetWindowRect(lrc_hwnd,&rect);

	memset(&tempBI,0,sizeof(BITMAPINFO));
	tempBI.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	tempBI.bmiHeader.biBitCount=32;
	tempBI.bmiHeader.biHeight=rect.bottom-rect.top;
	tempBI.bmiHeader.biWidth=rect.right-rect.left;
	tempBI.bmiHeader.biPlanes=1;
	tempBI.bmiHeader.biCompression=BI_RGB;
	tempBI.bmiHeader.biSizeImage=tempBI.bmiHeader.biWidth * tempBI.bmiHeader.biHeight * (tempBI.bmiHeader.biBitCount / 8);

	mdc_lrc=CreateCompatibleDC(hdc);
	VOID *ppvBits;
	mainBitmap_lrc=CreateDIBSection(mdc_lrc,&tempBI,DIB_RGB_COLORS,&ppvBits,NULL,NULL);
	oldBitmap_lrc=SelectObject(mdc_lrc,mainBitmap_lrc);

	GdipCreateFromHDC(mdc_lrc,&graphics_lrc);

	//-------------------------------
	GdipSetSmoothingMode(graphics_lrc,SmoothingModeHighQuality);
	GdipCreateFontFamilyFromName(GFont.FontName,NULL,&fontFam_lrc);
	GdipCreateStringFormat(NULL,NULL,&strFormat_lrc);
	GdipSetStringFormatAlign(strFormat_lrc,StringAlignmentCenter);

	GpRectF rectf1;
	rectf1.Height=GFont.FontSize;
	rectf1.Width=GFont.FontSize;
	rectf1.X=0;
	rectf1.Y=0;
	//�������
	LRC_BACK_COLOR lrc_color=GetLRCOKColor(drawlrc_color);
	GdipCreateLineBrushFromRect(&rectf1,lrc_color.LRC_BACK_COLOR1,lrc_color.LRC_BACK_COLOR2,LinearGradientModeVertical,WrapModeTileFlipXY,&Brush_lrc);

	rclayout_lrc.X=0;
	rclayout_lrc.Y=0;
	rclayout_lrc.Width=GetSystemMetrics(SM_CXSCREEN);	//��ȡ��Ļ���
	rclayout_lrc.Height=0;


	GdipCreatePath(FillModeAlternate,&strPath_lrc);
	GdipAddPathStringI(strPath_lrc,text,-1,fontFam_lrc,FontStyleBold,GFont.FontSize,&rclayout_lrc,strFormat_lrc);

	GdipFillPath(graphics_lrc,Brush_lrc,strPath_lrc);
	GdipDrawPath(graphics_lrc,pen_lrc,strPath_lrc);

	curWinLong=GetWindowLong(lrc_hwnd,GWL_EXSTYLE);
	if(curWinLong!=WS_EX_LAYERED)
		SetWindowLong(lrc_hwnd,GWL_EXSTYLE,curWinLong|WS_EX_LAYERED);

	srcPoint.x=0;
	srcPoint.y=0;
	winSize.cx=rect.right-rect.left;
	winSize.cy=rect.bottom-rect.top;

	blendFunc32bpp.AlphaFormat=AC_SRC_ALPHA;
	blendFunc32bpp.BlendFlags=0;
	blendFunc32bpp.BlendOp=AC_SRC_OVER;
	blendFunc32bpp.SourceConstantAlpha=255;
	POINT ptWinPos ={rect.left,rect.top };

	GdipDeleteGraphics(graphics_lrc);
	UpdateLayeredWindow(lrc_hwnd,hdc,&ptWinPos,&winSize,mdc_lrc,&srcPoint,0,&blendFunc32bpp,ULW_ALPHA);
	return TRUE;
}
//��ʾ��ʻ���
BOOL Show_DrawText(const WCHAR* text,HDC hdc){
	SelectObject(mdc_show,oldBitmap_show);
	DeleteObject(mainBitmap_show);
	DeleteObject(oldBitmap_show);
	DeleteDC(mdc_show);
	GdipDeleteFontFamily(fontFam_show);
	GdipDeleteStringFormat(strFormat_show);
	GdipDeletePath(strPath_show);
	GdipDeleteBrush(Brush_show);
	GdipDeletePen(pen_show);
	GdipDeleteGraphics(graphics_show);//�ͷ�graphicsռ�õ��ڴ�
	//�ͷ��ϴ�ʹ�õ���Դ
	BITMAPINFO tempBI;
	BLENDFUNCTION tempBlend;
	long curWinLong;
	SIZE winSize;
	POINT srcPoint;

	INT Screen_Width=GetSystemMetrics(SM_CXSCREEN);	//��Ļ���

	//��ȡ����λ��
	RECT rect;
	GetWindowRect(show_hwnd,&rect);
	//BITMAPINFO�ṹ��ֵ
	memset(&tempBI,0,sizeof(BITMAPINFO));
	tempBI.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	tempBI.bmiHeader.biBitCount=32;
	tempBI.bmiHeader.biHeight=rect.bottom-rect.top;
	tempBI.bmiHeader.biWidth=rect.right-rect.left;
	tempBI.bmiHeader.biPlanes=1;
	tempBI.bmiHeader.biCompression=BI_RGB;
	tempBI.bmiHeader.biSizeImage=tempBI.bmiHeader.biWidth * tempBI.bmiHeader.biHeight * (tempBI.bmiHeader.biBitCount / 8);
	//����GDI+DC
	mdc_show=CreateCompatibleDC(hdc);
	VOID *ppvBits;
	mainBitmap_show=CreateDIBSection(mdc_show,&tempBI,DIB_RGB_COLORS,&ppvBits,NULL,NULL);
	oldBitmap_show=SelectObject(mdc_show,mainBitmap_show);
	GdipCreateFromHDC(mdc_show,&graphics_show);

	GdipSetSmoothingMode(graphics_show,SmoothingModeHighQuality);
	GdipCreateFontFamilyFromName(GFont.FontName,NULL,&fontFam);
	GdipCreateStringFormat(NULL,NULL,&strFormat);
	GdipSetStringFormatAlign(strFormat,StringAlignmentCenter);

	GpRectF rectf1;
	rectf1.Height=GFont.FontSize;
	rectf1.Width=GFont.FontSize;
	rectf1.X=0;
	rectf1.Y=0;
	//�������   0xFFA5DAF7, 0xC8468DCC,0xFF010101

	GdipCreateLineBrushFromRect(&rectf1,0x90000000,0x90000000,LinearGradientModeVertical,WrapModeTileFlipY,&Brush_show);
	//����һ������ˢ
	//����һ����ߵı�ˢ
	rclayout.X=1;
	rclayout.Y=1;
	rclayout.Width=Screen_Width;
	rclayout.Height=0;
	GdipCreatePath(FillModeAlternate,&strPath);
	GdipAddPathStringI(strPath,text,-1,fontFam,FontStyleBold,GFont.FontSize,&rclayout,strFormat);

	GdipFillPath(graphics_show,Brush_show,strPath);
	GdipDrawPath(graphics_show,pen_show,strPath);
	//-----------------------------1
	GdipCreateLineBrushFromRect(&rectf1,0x30000000,0x30000000,LinearGradientModeVertical,WrapModeTileFlipXY,&Brush_show);
	//����һ������ˢ
	//����һ����ߵı�ˢ
	rclayout.X=1.5;
	rclayout.Y=1.5;
	rclayout.Width=Screen_Width;
	rclayout.Height=0;

	GdipCreatePath(FillModeAlternate,&strPath);
	GdipAddPathStringI(strPath,text,-1,fontFam,FontStyleBold,GFont.FontSize,&rclayout,strFormat);
	GdipFillPath(graphics_show,Brush_show,strPath);
	GdipDrawPath(graphics_show,pen_show,strPath);
	//-----------------------------2
	GdipCreateLineBrushFromRect(&rectf1,0x20000000,0x20000000,LinearGradientModeVertical,WrapModeTileFlipXY,&Brush_show);
	//����һ������ˢ
	//����һ����ߵı�ˢ
	rclayout.X=2;
	rclayout.Y=2;
	rclayout.Width=Screen_Width;
	rclayout.Height=0;
	GdipCreatePath(FillModeAlternate,&strPath);
	GdipAddPathStringI(strPath,text,-1,fontFam,FontStyleBold,GFont.FontSize,&rclayout,strFormat);
	GdipFillPath(graphics_show,Brush_show,strPath);
	GdipDrawPath(graphics_show,pen_show,strPath);
	//----------------------------3
	GdipCreateLineBrushFromRect(&rectf1,0x10000000,0x10000000,LinearGradientModeVertical,WrapModeTileFlipXY,&Brush_show);
	//����һ������ˢ
	//����һ����ߵı�ˢ
	rclayout.X=2.2;
	rclayout.Y=2.2;
	rclayout.Width=Screen_Width;
	rclayout.Height=0;
	GdipCreatePath(FillModeAlternate,&strPath);
	GdipAddPathStringI(strPath,text,-1,fontFam,FontStyleBold,GFont.FontSize,&rclayout,strFormat);
	GdipFillPath(graphics_show,Brush_show,strPath);
	GdipDrawPath(graphics_show,pen_show,strPath);
	//----------------------------4
	GdipCreateLineBrushFromRect(&rectf1,0x40000000,0x40000000,LinearGradientModeVertical,WrapModeTileFlipXY,&Brush_show);
	//����һ������ˢ
	//����һ����ߵı�ˢ
	rclayout.X=-1;
	rclayout.Y=-1;
	rclayout.Width=Screen_Width;
	rclayout.Height=0;
	GdipCreatePath(FillModeAlternate,&strPath);
	GdipAddPathStringI(strPath,text,-1,fontFam,FontStyleBold,GFont.FontSize,&rclayout,strFormat);
	GdipFillPath(graphics_show,Brush_show,strPath);
	GdipDrawPath(graphics_show,pen_show,strPath);
	//---------------------------5
	GdipCreateLineBrushFromRect(&rectf1,0x30000000,0x30000000,LinearGradientModeVertical,WrapModeTileFlipXY,&Brush_show);
	//����һ������ˢ
	//����һ����ߵı�ˢ
	rclayout.X=-1.5;
	rclayout.Y=-1.5;
	rclayout.Width=Screen_Width;
	rclayout.Height=0;
	GdipCreatePath(FillModeAlternate,&strPath);
	GdipAddPathStringI(strPath,text,-1,fontFam,FontStyleBold,GFont.FontSize,&rclayout,strFormat);
	GdipFillPath(graphics_show,Brush_show,strPath);
	GdipDrawPath(graphics_show,pen_show,strPath);
	//---------------------------6
	GdipCreateLineBrushFromRect(&rectf1,GFont.ForeColor1,GFont.ForeColor2,LinearGradientModeVertical,WrapModeTileFlipXY,&Brush_show);
	//���һ����ˢ
	GdipCreatePen1(0x90000000,1,UnitDocument,&pen_show);
	//����һ����ߵı�ˢ
	rclayout.X=0;
	rclayout.Y=0;
	rclayout.Width=Screen_Width;
	rclayout.Height=0;
	GdipCreatePath(FillModeAlternate,&strPath);
	GdipAddPathStringI(strPath,text,-1,fontFam,FontStyleBold,GFont.FontSize,&rclayout,strFormat);
	GdipFillPath(graphics_show,Brush_show,strPath);
	GdipDrawPath(graphics_show,pen_show,strPath);
	curWinLong=GetWindowLong(show_hwnd,GWL_EXSTYLE);
	if(curWinLong!=WS_EX_LAYERED)
		SetWindowLong(show_hwnd,GWL_EXSTYLE,curWinLong|WS_EX_LAYERED);
	srcPoint.x=0;
	srcPoint.y=0;
	winSize.cx=rect.right-rect.left;
	winSize.cy=rect.bottom-rect.top;

	blendFunc32bpp.AlphaFormat=AC_SRC_ALPHA;
	blendFunc32bpp.BlendFlags=0;
	blendFunc32bpp.BlendOp=AC_SRC_OVER;
	blendFunc32bpp.SourceConstantAlpha=255;
	POINT ptWinPos ={rect.left,rect.top };

	GdipDeleteGraphics(graphics_show);
	UpdateLayeredWindow(show_hwnd,hdc,&ptWinPos,&winSize,mdc_show,&srcPoint,0,&blendFunc32bpp,ULW_ALPHA);
	return TRUE;
}
//���Ʊ�������
BOOL MakeTrans(HWND hWnd,HDC hdc){
	BITMAPINFO tempBI;
	BLENDFUNCTION tempBlend;
	UINT lngHeight=0,lngWidth=0;
	LONG curWinLong=0;
	GpImage *img=NULL,*img1=NULL,*img2=NULL;
	GpGraphics *graphics=NULL;
	SIZE winSize;
	POINT srcPoint;
	//TCHAR *pngPath=NULL,*pngPath1=NULL,*pngPath2=NULL;	//����͸��ͼƬ·��
	//TCHAR szFilePath[MAX_PATH+1];	//��ǰ����Ŀ¼
	//GetModuleFileName(NULL,szFilePath,MAX_PATH);	//��ȡ·�������ļ���
	//(_tcsrchr(szFilePath, _T('\\')))[1] = 0;	//ɾ���ļ�����ֻ����·��
	INT MidWidth,RLeft;
	//��ȡ����λ��
	RECT rect;
	GetWindowRect(hWnd,&rect);

	//pngPath=L"c:\\left.png";
	//pngPath1=L"c:\\mid.png";
	//pngPath2=L"c:\\right.png";
	memset(&tempBI,0,sizeof(BITMAPINFO));
	tempBI.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	tempBI.bmiHeader.biBitCount=32;
	tempBI.bmiHeader.biHeight=rect.bottom-rect.top;
	tempBI.bmiHeader.biWidth=rect.right-rect.left;
	tempBI.bmiHeader.biPlanes=1;
	tempBI.bmiHeader.biCompression=BI_RGB;
	tempBI.bmiHeader.biSizeImage=tempBI.bmiHeader.biWidth * tempBI.bmiHeader.biHeight * (tempBI.bmiHeader.biBitCount / 8);

	mdc=CreateCompatibleDC(hdc);
	VOID *ppvBits;
	mainBitmap=CreateDIBSection(mdc,&tempBI,DIB_RGB_COLORS,&ppvBits,NULL,NULL);

	oldBitmap=SelectObject(mdc,mainBitmap);

	GdipCreateFromHDC(mdc,&graphics);
	//��Ϊ����Դ��ȡ��
	ImageFromIDResource(hInstance,IDB_BACK_LEFT,_T("PNG"),img);
	ImageFromIDResource(hInstance,IDB_BACK_MID,_T("PNG"),img1);
	ImageFromIDResource(hInstance,IDB_BACK_RIGHT,_T("PNG"),img2);
	//GdipLoadImageFromFile(pngPath,&img);
	//GdipLoadImageFromFile(pngPath1,&img1);
	//GdipLoadImageFromFile(pngPath2,&img2);
	GdipGetImageHeight(img,&lngHeight);
	GdipGetImageWidth(img,&lngWidth);
	GdipDrawImageRect(graphics,img,0,0,lngWidth,lngHeight+2);

	MidWidth=(rect.right-rect.left)-lngWidth*2;

	GdipDrawImageRect(graphics,img1,lngWidth,0,MidWidth,lngHeight);

	RLeft=MidWidth + lngWidth;

	GdipDrawImageRect(graphics,img2,RLeft,0,lngWidth,lngHeight+2);

	curWinLong=GetWindowLong(hWnd,GWL_EXSTYLE);
	if(curWinLong!=WS_EX_LAYERED)
		SetWindowLong(hWnd,GWL_EXSTYLE,curWinLong|WS_EX_LAYERED);
	SetWindowPos(hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);

	srcPoint.x=0;
	srcPoint.y=0;
	winSize.cx=rect.right-rect.left;
	winSize.cy=rect.bottom-rect.top;

	blendFunc32bpp.AlphaFormat=AC_SRC_ALPHA;
	blendFunc32bpp.BlendFlags=0;
	blendFunc32bpp.BlendOp=AC_SRC_OVER;
	blendFunc32bpp.SourceConstantAlpha=255;

	POINT ptWinPos ={rect.left,rect.top };

	GdipDisposeImage(img);
	GdipDeleteGraphics(graphics);	//�رջ���
	UpdateLayeredWindow(hWnd,hdc,&ptWinPos,&winSize,mdc,&srcPoint,0,&blendFunc32bpp,ULW_ALPHA);
	return TRUE;
}
//����ͨ�ô���
//�����Ĵ��ھ��win_hwnd
//��������lpszClassName
//�����¼�������proc
//����λ��win_rect
BOOL CreateWindowUniversal(HWND &win_hwnd,LPCWSTR lpszClassName,WNDPROC proc,RECT win_rect){
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW | WS_EX_TOPMOST;
	wcex.lpfnWndProc	= proc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GNORE));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= lpszClassName;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_GNORE));
	if(RegisterClassEx(&wcex)==0)	//ע�ᴰ����
		return FALSE;
	win_hwnd=CreateWindow(lpszClassName, _T("LRC���"), WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_POPUP,win_rect.left, win_rect.top, win_rect.right-win_rect.left, win_rect.bottom-win_rect.top, NULL, NULL, NULL, NULL);
	if(!win_hwnd)
		return FALSE;
	SetWindowLong(win_hwnd,GWL_EXSTYLE,WS_EX_TOOLWINDOW);	//������������ʾ
	ShowWindow(win_hwnd, SW_HIDE);
	UpdateWindow(win_hwnd);
	return TRUE;
}
//����Դ����img��gdi
BOOL ImageFromIDResource(HINSTANCE hInst,UINT nID, LPCTSTR sTR,GpImage * &pImg){
	HRSRC hRsrc =FindResource(hInst,MAKEINTRESOURCE(nID),sTR);	//type
	if(!hRsrc)
		return false;
	DWORD len=SizeofResource(hInst,hRsrc);
	BYTE* lpRsrc=(BYTE*)LoadResource(hInst, hRsrc);
	if(!lpRsrc)
		return false;
	HGLOBAL m_hMem=GlobalAlloc(GMEM_FIXED,len);
	BYTE *pmem=(BYTE*)GlobalLock(m_hMem);
	memcpy(pmem,lpRsrc,len);
	IStream* pstm;
	CreateStreamOnHGlobal(m_hMem,FALSE,&pstm);
	// load from stream
	//pImg=Gdiplus::Image::FromStream(pstm);
	GdipLoadImageFromStream(pstm,&pImg);
	// free/release stuff
	GlobalUnlock(m_hMem);
	pstm->Release();
	FreeResource(lpRsrc);
	return true;
}
//��ȡ����OK��ɫ
LRC_BACK_COLOR GetLRCOKColor(LRC_COLOR color){
	LRC_BACK_COLOR LRC_COLOR;
	switch(color){
		case LRC_COLOR_Blue:
			LRC_COLOR.LRC_BACK_COLOR1=0xFFBCF9FC;
			LRC_COLOR.LRC_BACK_COLOR2=0xFF67F0FC;
			break;
		case LRC_COLOR_Green:
			LRC_COLOR.LRC_BACK_COLOR1=0xFFDCFEAE;
			LRC_COLOR.LRC_BACK_COLOR2=0xFFE4FE04;
			break;
		case LRC_COLOR_Red:
			LRC_COLOR.LRC_BACK_COLOR1=0xFFFEFE65;
			LRC_COLOR.LRC_BACK_COLOR2=0xFFFEA10F;
			break;
		case LRC_COLOR_White:
			LRC_COLOR.LRC_BACK_COLOR1=0xFF62DDFF;
			LRC_COLOR.LRC_BACK_COLOR2=0xFF229CFE;
			break;
		case LRC_COLOR_Yellow:
			LRC_COLOR.LRC_BACK_COLOR1=0xFFFFFF6E;
			LRC_COLOR.LRC_BACK_COLOR2=0xFFFEA10F;
			break;
		default:
			LRC_COLOR.LRC_BACK_COLOR1=0xFFBA0000;
			LRC_COLOR.LRC_BACK_COLOR2=0xFFFF9301;
			break;
	}
	return LRC_COLOR;
}
//��ȡ��ʾ�����ɫ
LRC_BACK_COLOR GetShowColor(LRC_COLOR color){
	LRC_BACK_COLOR show_color;
	switch(color){
	case LRC_COLOR_Blue:
		show_color.LRC_BACK_COLOR1=0xFF013C8F;
		show_color.LRC_BACK_COLOR2=0xFF0198D4;
		break;
	case LRC_COLOR_Green:
		show_color.LRC_BACK_COLOR1=0xFF87F321;
		show_color.LRC_BACK_COLOR2=0xFF0E6700;
		break;
	case LRC_COLOR_Red:
		show_color.LRC_BACK_COLOR1=0xFFFECEFC;
		show_color.LRC_BACK_COLOR2=0xFFE144CD;
		break;
	case LRC_COLOR_White:
		show_color.LRC_BACK_COLOR1=0xFFFBFBFA;
		show_color.LRC_BACK_COLOR2=0xFFCBCBCB;
		break;
	case LRC_COLOR_Yellow:
		show_color.LRC_BACK_COLOR1=0xFFFE7A00;
		show_color.LRC_BACK_COLOR2=0xFFFF0000;
		break;
	}
	return show_color;
}
//��ʱ���ص�����
void CALLBACK TimerProc(HWND hWnd,UINT nMsg,UINT nTimerid,DWORD dwTime){
	if(nTimerid==TIMEWINDMONITOR){
		POINT udtPoint;
		GetCursorPos(&udtPoint);	//��ȡ�������
		HWND lonCurHWND=WindowFromPoint(udtPoint);	//��ȡ����ϵľ��
		if(lonCurHWND!=show_hwnd&&lonCurHWND!=lrc_hwnd&&lonCurHWND!=back_hwnd){
			ShowWindow(back_hwnd,SW_HIDE);
		}else{
			ShowWindow(back_hwnd,SW_SHOWNOACTIVATE);
		}
	}else if(nTimerid==TIMELRC){
		RECT lrc_rect_tmp;
		GetWindowRect(lrc_hwnd,&lrc_rect_tmp);
		INT ScreenWidth=GetSystemMetrics(SM_CXSCREEN);	//��ȡ��Ļ���
		INT Width=(lrc_rect_tmp.right-lrc_rect_tmp.left)+(str_width/LrcTime/50);
		MoveWindow(lrc_hwnd,lrc_rect_tmp.left,lrc_rect_tmp.top,Width,lrc_rect_tmp.bottom-lrc_rect_tmp.top,TRUE);
		//SetWindowPos(lrc_hwnd,NULL,NULL,NULL,Width,lrc_rect_tmp.bottom-lrc_rect_tmp.top,SWP_NOMOVE|SWP_NOZORDER);
		lrc_DrawText(LRCText,mdc_lrc);
		if(Width>((ScreenWidth-str_width)/2)+str_width){
			KillTimer(lrc_hwnd,TIMELRC);
		}
	}
}

BOOL isMouseCoordinate(POINT mouse_point){
	int ScreenWidth=GetSystemMetrics(SM_CXSCREEN);	//��ȡ��Ļ���
	int Rwheight=GetSystemMetrics(SM_CYFULLSCREEN);	//��ȡ�ɼ���Ļ�߶�(������������)
	if(mouse_point.x<0||mouse_point.y<0||mouse_point.x>ScreenWidth||mouse_point.y>Rwheight){
		return FALSE;
	}
	return TRUE;
}
BOOL isMouseCoordinate(int mouse_x,int mouse_y){
	POINT point;
	point.x=mouse_x;
	point.y=mouse_y;
	return isMouseCoordinate(point);
}

VOID SetZOrder(){
	//SetParent(show_hwnd,lrc_hwnd);
	//SetParent(back_hwnd,show_hwnd);
	//SetWindowPos(back_hwnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	SetWindowPos(back_hwnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	SetWindowPos(show_hwnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	SetWindowPos(lrc_hwnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	/*EnableWindow(back_hwnd,FALSE);
	EnableWindow(show_hwnd,FALSE);
	EnableWindow(lrc_hwnd,FALSE);*/
}