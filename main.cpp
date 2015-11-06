#pragma comment(lib,"imm32")
#pragma comment(lib,"comctl32")

#include <windows.h>
#include <commctrl.h>
#include <imm.h>
#include "resource.h"

CHAR szClassName[]="window";

HWND hEdit,hList,hCeck;

LRESULT CALLBACK WndProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	static HFONT hFont;
	switch (msg)
	{
	case WM_COMMAND:
		if(HIWORD(wParam) == EN_CHANGE || LOWORD(wParam) == 101)
		{
			TCHAR szEdit[256];
			ListView_DeleteAllItems(hList);
			if(GetWindowTextLength(hEdit)==0)break;
			GetWindowText(hEdit,szEdit,256);
			for (int i = 1; i <= 100; ++i)
			{
				CHAR szText[1024];
				LoadString(
					GetModuleHandle(NULL),
					i,
					szText,
					sizeof(szText)/sizeof(szText[0])
					);
				BOOL bCeck=(
					BST_CHECKED==SendMessage(
					hCeck,BM_GETCHECK,0,0)
					);
				if(bCeck?
					((BOOL)strstr(szText,szEdit))
					:
				(CSTR_EQUAL == CompareString(
					LOCALE_SYSTEM_DEFAULT,
					NORM_IGNOREKANATYPE,
					szText,
					strlen(szEdit),
					szEdit,
					strlen(szEdit))
					))
				{
					char* pszTemp = NULL;
					char *token;
					token = strtok_s(szText, "\n", &pszTemp);
					token = strtok_s(NULL, "\n", &pszTemp);
					LV_ITEM		item;
					item.mask = LVIF_TEXT;
					item.pszText=token;
					item.iItem = 0;
					item.iSubItem = 0;
					ListView_InsertItem(hList, &item);
					token = strtok_s(NULL, "\n", &pszTemp);
					item.pszText=token;
					item.iItem = 0;
					item.iSubItem = 1;
					ListView_SetItem(hList, &item);
					token = strtok_s(NULL, "\n", &pszTemp);
					item.pszText=token;
					item.iItem = 0;
					item.iSubItem = 2;
					ListView_SetItem(hList, &item);
				}
			}
		}
		break;
	case WM_SETFOCUS:
		SetFocus(hEdit);
		SendMessage(hEdit,EM_SETSEL,0,-1);
		break;
	case WM_SIZE:
		MoveWindow(hEdit,5,5,LOWORD(lParam)-10-64,28,TRUE);
		MoveWindow(hCeck,LOWORD(lParam)-10-64+10,5,64,28,TRUE);
		MoveWindow(hList,5,38,LOWORD(lParam)-10,HIWORD(lParam)-43,TRUE);
		break;
	case WM_CREATE:
		InitCommonControls();
		hEdit=CreateWindowEx(
			WS_EX_CLIENTEDGE,
			"EDIT",
			0,
			WS_CHILD|WS_VISIBLE|WS_TABSTOP,
			0,
			0,
			0,
			0,
			hWnd,
			(HMENU)100,
			((LPCREATESTRUCT)(lParam))->hInstance,
			NULL);
		hCeck=CreateWindow(
			"BUTTON",
			"&Crazy",
			WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX|WS_TABSTOP,
			0,
			0,
			0,
			0,
			hWnd,
			(HMENU)101,
			((LPCREATESTRUCT)(lParam))->hInstance,
			NULL);
		hList=CreateWindowEx(
			WS_EX_CLIENTEDGE,
			WC_LISTVIEW,
			0,
			WS_CHILD|WS_VISIBLE|LVS_REPORT|WS_TABSTOP,
			0,
			0,
			0,
			0,
			hWnd,
			(HMENU)102,
			((LPCREATESTRUCT)(lParam))->hInstance,
			NULL);
		{//リストビューの選択を1行全体に
			DWORD  dwStyle = ListView_GetExtendedListViewStyle(hList);
			dwStyle |=  LVS_EX_FULLROWSELECT|LVS_EX_INFOTIP;
			ListView_SetExtendedListViewStyle(hList, dwStyle); 
		}
		{//コントロールのフォントを明朝体に設定
			hFont=CreateFont(
				14,
				0,
				0,
				0,
				FW_NORMAL,
				FALSE,
				FALSE,
				0,
				SHIFTJIS_CHARSET,
				OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY,
				DEFAULT_PITCH,
				"ＭＳ Ｐ明朝");
			SendMessage(hEdit,WM_SETFONT,(WPARAM)hFont,0);
			SendMessage(hList,WM_SETFONT,(WPARAM)hFont,0);
		}
		{//リストビューのヘッダー
			LV_COLUMN	lvcol;
			lvcol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
			lvcol.fmt = LVCFMT_LEFT;
			lvcol.cx = 256;
			lvcol.pszText = "上の句";
			lvcol.iSubItem = 0;
			ListView_InsertColumn(hList, 0, &lvcol);
			
			lvcol.fmt = LVCFMT_LEFT;
			lvcol.cx = 256;
			lvcol.pszText = "下の句";
			lvcol.iSubItem = 1;
			ListView_InsertColumn(hList, 1, &lvcol);
			
			lvcol.fmt = LVCFMT_LEFT;
			lvcol.cx = 128;
			lvcol.pszText = "歌人";
			lvcol.iSubItem = 2;
			ListView_InsertColumn(hList, 2, &lvcol);
		}
		{//IMEをON
			HIMC hIMC = ImmGetContext( hWnd );
			ImmSetOpenStatus( hIMC, TRUE );
			ImmReleaseContext( hWnd, hIMC );
		}
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		DeleteObject(hFont);
		PostQuitMessage(0);
		break;
	default:
		return(DefDlgProc(hWnd,msg,wParam,lParam));
}
return (0L);
}


EXTERN_C void __cdecl WinMainCRTStartup()
{
	MSG msg;
	HINSTANCE hInstance=GetModuleHandle(0);
	WNDCLASS wndclass={
		0,
			WndProc,
			0,
			DLGWINDOWEXTRA,
			hInstance,
			0,
			LoadCursor(0,IDC_ARROW),
			0,
			0,
			szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd=CreateWindow(szClassName,
		"百人一首検索くん",
		WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL);
	ShowWindow(hWnd,SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while(GetMessage(&msg,0,0,0))
	{
		if(!IsDialogMessage(hWnd,&msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	ExitProcess(msg.wParam);
}

#if _DEBUG
void main(){}
#endif
