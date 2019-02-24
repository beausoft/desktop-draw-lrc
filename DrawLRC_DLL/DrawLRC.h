#define TIMEWINDMONITOR 1000	//���ڼ���TIMERID
#define TIMELRC 1001			//���Ч��

BOOL CreateWindowUniversal(HWND &win_hwnd,LPCWSTR lpszClassName,WNDPROC proc,RECT win_rect);
BOOL ImageFromIDResource(HINSTANCE hInst,UINT nID, LPCTSTR sTR,GpImage * &pImg);//����Դimg��gdi
BOOL CreateBackWindow();
BOOL CreateShowWindow();
BOOL CreateLrcWindow();
//����������Ϣ������
LRESULT CALLBACK Back_WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK Show_WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK Lrc_WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL MakeTrans(HWND hWnd,HDC hdc);	//��ͼ��������
BOOL Show_DrawText(const WCHAR* text,HDC hdc);	//��ʾ��ʷ���
BOOL lrc_DrawText(const WCHAR* text,HDC hdc);	//����OK����
BOOL sethInstance(HINSTANCE hIns);
HWND getBACK_HWND();
//��������ṹ
struct GdiFont{
public:
	WCHAR* FontName;
	REAL FontSize;
	long ForeColor1;
	long ForeColor2;
	long LineColor;
	byte FontWidth;
	bool FontBold;
	long OkColor1;
	long OKColor2;
};
//���忨��ok��ɫ�ṹ
struct LRC_BACK_COLOR{
public:
	ARGB LRC_BACK_COLOR1;
	ARGB LRC_BACK_COLOR2;
};
//��ɫö��
enum LRC_COLOR{
	LRC_COLOR_Blue=1,	//��ɫ
	LRC_COLOR_Green=2,//��ɫ
	LRC_COLOR_Red=3,//��ɫ
	LRC_COLOR_White=4,//��ɫ
	LRC_COLOR_Yellow=5//��ɫ
};

LRC_BACK_COLOR GetLRCOKColor(LRC_COLOR color);
LRC_BACK_COLOR GetShowColor(LRC_COLOR color);
//��ʱ���ص�����
void CALLBACK TimerProc(HWND hWnd,UINT nMsg,UINT nTimerid,DWORD dwTime);
VOID SetZOrder();
//�ж���������Ƿ���ȷ
BOOL isMouseCoordinate(POINT mouse_point);
BOOL isMouseCoordinate(int mouse_x,int mouse_y);