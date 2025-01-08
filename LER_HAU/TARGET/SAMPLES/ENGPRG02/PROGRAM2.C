/*
Product  :  Text-To-Speech Software Development Kit
Version  :  4.00
Program  :  PROGRAM2.c
Purpose  :  This program illustrates how to use multiple threads in an
            application. When it starts, first generated thread speaks 
            a text saying which number the thread handle has. It offers
            a dialog box to edit a text, the contents of which are editted 
            is spoken out. It also introduces an auditive help function.
Comments :  Comments to the source code are given as they pertain to
            the usage of TTS.

Copyright (c) 1996, Lernout & Hauspie Speech Products.
All rights reserved.
*/
                                                                                                                                          
#include "program2.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "ttssdk32.h"

/*
 *  Global variable definition
 */
HANDLE      ghModule;
HWND        ghwndMain = NULL;
HWND        ghwndCurrent = NULL;
HBRUSH      ghbrWhite, ghbrBlack;
int         cmdDemo   = 0;
int         nThreadCnt=0;
BOOL        bKillMe = FALSE;
HINSTANCE   ghInst;
HMENU       hMenu, hMenuWindow;
char        gszString[128];
char        gszAppName[20];
HWND        hWndMain;
HWND        hEditWnd;

HTTSINSTANCE    hTtsInst;


typedef struct _ThreadBlockInfo {
        HANDLE          hThread;
        BOOL            bKillThrd;
        HWND            hwndClient;
        HWND            hwndThreadWindow;
        LONG            lThreadId;
        char            CaptionBarText[SIZEOFCAPTIONTEXT];
        RECT            rcClient;
        HDC             hdcThreadWindow;
        HTTSINSTANCE    hTtsInst;
} THREADBLOCKINFO, *PTHREADBLOCKINFO;


typedef struct _node {
    THREADBLOCKINFO ThreadWindow;
    HANDLE          hNext;
} NODE, *PNODE;


/*
 *  Forward declarations.
 */
BOOL InitializeApp(void);
int WINAPI MainWndProc(HWND, UINT, DWORD, LONG);
int WINAPI ThreadWndProc(HWND, UINT, DWORD, LONG);
LONG StartThread(PTHREADBLOCKINFO);
int InputAndPlayText(HWND hWnd);
int WINAPI InputAndPlayDlg(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

/***************************************************************************\
*   main
\***************************************************************************/
int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nShowCmd)
{
    MSG     msg;

    ghInst = hInstance;

    ghModule = GetModuleHandle(NULL);

    if (!InitializeApp()) 
    {
        MessageBox(ghwndMain, "MLTITHRD: InitializeApp failure!", "Error", MB_OK);
        return 0;
    }

/*
 *  Generate one thread and each thread produces playback of a text
 */

    PostMessage(ghwndMain, WM_COMMAND, IDM_T_THREAD, 0);
    PostMessage(ghwndMain, WM_COMMAND, IDM_TILE, 0);

    while (GetMessage(&msg, NULL, 0, 0)) 
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 1;
}


/***************************************************************************\
*   InitializeApp
\***************************************************************************/

BOOL InitializeApp(void)
{
    WNDCLASS wc;

    srand(51537);
    
    ghbrWhite = CreateSolidBrush(0x00FFFFFF);
    ghbrBlack = CreateSolidBrush(0x00000000);

    wc.style            = CS_OWNDC;
    wc.lpfnWndProc      = (WNDPROC) MainWndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = sizeof(LONG);
    wc.hInstance        = ghModule;
    wc.hIcon            = LoadIcon(ghModule, "PROGRAMICON");
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground    = ghbrWhite;
    wc.lpszMenuName     = "MainMenu";
    wc.lpszClassName    = "MltithrdClass";

    if (!RegisterClass(&wc))
        return FALSE;

    wc.style            = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc      = (WNDPROC) ThreadWndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    wc.hInstance        = ghModule;
    wc.hIcon            = LoadIcon(ghModule, "PROGRAMICON");
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground    = ghbrWhite;
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = "ThreadClass";

    if (!RegisterClass(&wc))
        return FALSE;

    hMenu = LoadMenu(ghModule, "MainMenu");
    hMenuWindow = GetSubMenu(hMenu, 1);

    ghwndMain = CreateWindowEx(0L, "MltithrdClass", "L&H TTS Sample Program 2",
                WS_OVERLAPPED | WS_CAPTION | WS_BORDER | WS_THICKFRAME |
                WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_CLIPCHILDREN |
                WS_VISIBLE | WS_SYSMENU,
                80, 70, 400, 300,
                NULL, hMenu, ghModule, NULL);

    if (ghwndMain == NULL)
        return FALSE;

    SetWindowLong(ghwndMain, GWL_USERDATA, 0L);

    SetFocus(ghwndMain);    /* set initial focus */

    return TRUE;
}


/***************************************************************************\
*   MainWndProc
\***************************************************************************/
int WINAPI MainWndProc(
    HWND hwnd,
    UINT message,
    DWORD wParam,
    LONG lParam)
{
    static int         	iCount=1;
    static HWND        	hwndClient;
    CLIENTCREATESTRUCT 	clientcreate;
    HWND                hwndChildWindow;
    int                 nRc;
    FARPROC             lpfnDIALOGMsgProc;


    switch (message) 
    {
        case WM_CREATE:
            SetWindowLong(hwnd, 0, (LONG)NULL);
            clientcreate.hWindowMenu  = hMenuWindow;
            clientcreate.idFirstChild = 1;
            hwndClient = CreateWindow("MDICLIENT", NULL,
                                      WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE,
                                      0,0,0,0, hwnd, NULL, ghModule,
                                      (LPVOID)&clientcreate);
            return 0L;

        case WM_DESTROY: 
        {
            HANDLE hHead, hTmp;
            PNODE  pHead;

            bKillMe = TRUE;

            hHead = (HANDLE) GetWindowLong(hwnd, 0);
            if (hHead) 
            {
                if ((pHead = (PNODE) LocalLock(hHead))==NULL)
                MessageBox(ghwndMain, "Failed in LocalLock!", "Error", MB_OK);
                while (pHead->hNext != NULL) 
                {
                    hTmp = hHead;
                    hHead = pHead->hNext;
                    LocalUnlock(hTmp);

                    if (LocalFree(hTmp)!=NULL)
                        MessageBox(ghwndMain, "Failed in LocalFree!", "Error", MB_OK);
                    if ((pHead = (PNODE) LocalLock(hHead))==NULL)
                        MessageBox(ghwndMain, "Failed in LocalLock, hHead!", "Error", MB_OK);
                }
              LocalUnlock(hHead);
              if (LocalFree(hHead)!=NULL)
                MessageBox(ghwndMain, "LocalFree failed to free hHead!", "Error", MB_OK);
            }
            PostQuitMessage(0);
            return 0L;
        }

        case WM_COMMAND:
            switch (LOWORD(wParam)) 
            {
                case IDM_F_EXIT:
                    PostMessage(ghwndMain,WM_CLOSE,0,0);
                    break;
                case IDM_TILE:
                    SendMessage(hwndClient, WM_MDITILE, 0L, 0L);
                    return 0L;
                case IDM_CASCADE:
                    SendMessage(hwndClient, WM_MDICASCADE, 0L, 0L);
                    return 0L;
                case IDM_T_THREAD: 
                {
                    HANDLE          hNode, hHead;
                    HANDLE          hThrd;
                    PNODE           pNode;
                    MDICREATESTRUCT mdicreate;

                    hNode = LocalAlloc(LHND, (WORD) sizeof(NODE));
                    if (hNode) 
                    { 
                        if ((pNode = (PNODE)LocalLock(hNode))==NULL)
                            MessageBox(ghwndMain, "Failed in LocalLock, hNode!", "Error", MB_OK);
                        wsprintf((LPSTR)&(pNode->ThreadWindow.CaptionBarText), "Thread Window %d", iCount);

                        mdicreate.szClass = "ThreadClass";
                        mdicreate.szTitle = (LPSTR)&(pNode->ThreadWindow.CaptionBarText);
                        mdicreate.hOwner  = ghModule;
                        mdicreate.x       =
                        mdicreate.y       =
                        mdicreate.cx      =
                        mdicreate.cy      = CW_USEDEFAULT;
                        mdicreate.style   = 0l;
                        mdicreate.lParam  = 0L;

                        /*Create Child Window*/
                        hwndChildWindow = (HANDLE) SendMessage(hwndClient, WM_MDICREATE,
                                           0L,
                                           (LONG)(LPMDICREATESTRUCT)&mdicreate);

                        if (hwndChildWindow == NULL) 
                        {
                            MessageBox(ghwndMain, "Failed in Creating Thread Window!", "Error", MB_OK);
                            return 0L;
                        }

                        pNode->ThreadWindow.hwndClient       = hwndClient;
                        pNode->ThreadWindow.hwndThreadWindow = hwndChildWindow;
                        hHead = (HANDLE)GetWindowLong(hwnd, 0);
                        pNode->hNext = hHead;
                        SetWindowLong(hwnd, 0, (LONG) hNode);

                        // Create the thread suspended so we can alter its priority
                        // before it begins to run.

                        hThrd = CreateThread(NULL, 0,
                                            (LPTHREAD_START_ROUTINE)StartThread,
                                            &pNode->ThreadWindow,
                                            CREATE_SUSPENDED,
                                            (LPDWORD) &pNode->ThreadWindow.lThreadId );
                        if (hThrd) 
                        {

                            pNode->ThreadWindow.hThread = hThrd;
                            iCount++;
                            nThreadCnt++;

                            if (nThreadCnt>=1)
                            {
                                EnableMenuItem(hMenu, IDM_T_SPEAK, MF_ENABLED);
                            }

                            SetThreadPriority(hThrd, THREAD_PRIORITY_BELOW_NORMAL);
                            ResumeThread(hThrd);
                        } 
                        else 
                        {
                            MessageBox(ghwndMain, "Create Thread Failed!", "Error", MB_OK);
                        }
                        LocalUnlock(hNode);
                    }
                    else 
                    {
                        MessageBox(ghwndMain, "Failed to Allocate Node!", "Error", MB_OK);
                    }
                    return 0L;
                }

                case IDM_H_ABOUT:
                    lpfnDIALOGMsgProc = MakeProcInstance((FARPROC)About, ghInst);
                    nRc = DialogBox(ghInst, MAKEINTRESOURCE(100), ghwndMain, lpfnDIALOGMsgProc);
                    FreeProcInstance(lpfnDIALOGMsgProc);
                    return 0L;

                case IDM_T_SPEAK:
                {
                    PNODE       pHead;
                    HANDLE      hHead, hTmp;

                    // now find match
                    hHead = (HANDLE) GetWindowLong(ghwndMain, 0);
                    if (hHead) 
                    {
                        if ((pHead = (PNODE)LocalLock(hHead))==NULL)
                            MessageBox(ghwndMain, "Failed in LocalLock!", "Error", MB_OK);

                        while ((pHead->ThreadWindow.hwndThreadWindow != ghwndCurrent) &&
                               (pHead->hNext != NULL)) 
                        {
                            hTmp = hHead;
                            hHead = pHead->hNext;
                            if ((pHead = (PNODE) LocalLock(hHead))==NULL)
                                MessageBox(ghwndMain, "Failed in LocalLock!", "Error", MB_OK);
                        }
                        if (pHead->ThreadWindow.hwndThreadWindow == ghwndCurrent) 
                        {
                            /* Play a text	*/
                            InputAndPlayText(ghwndCurrent);
                        } 
                    }
                }
                return 0L;
                
                default:
                    return DefFrameProc(hwnd,  hwndClient, message, wParam, lParam);
            }

            default:
                return DefFrameProc(hwnd,  hwndClient, message, wParam, lParam);
    }
}


/***************************************************************************\
*   ThreadWndProc
\***************************************************************************/
int WINAPI ThreadWndProc(
    HWND hwnd,
    UINT message,
    DWORD wParam,
    LONG lParam)
{

    int     nRc=0;

    switch (message) 
    {
        case WM_SIZE:

        case WM_CREATE: 
        {
            PTHREADBLOCKINFO pThreadBlockInfo;
            PNODE            pHead;
            HANDLE           hHead, hTmp;

            ghwndCurrent = hwnd;

            // now find match
            hHead = (HANDLE) GetWindowLong(ghwndMain, 0);
            if (hHead) 
            {
                if ((pHead = (PNODE)LocalLock(hHead))==NULL)
                    MessageBox(ghwndMain, "Failed in LocalLock!", "Error", MB_OK);

                while ((pHead->ThreadWindow.hwndThreadWindow != hwnd) &&
                       (pHead->hNext != NULL)) 
                {
                    hTmp = hHead;
                    hHead = pHead->hNext;
                    LocalUnlock(hTmp);

                    if ((pHead = (PNODE) LocalLock(hHead))==NULL)
                        MessageBox(ghwndMain, "Failed in LocalLock!", "Error", MB_OK);
                }
                if (pHead->ThreadWindow.hwndThreadWindow == hwnd) 
                {
                    pThreadBlockInfo = &pHead->ThreadWindow;
                    goto Thread_Found;
                } 
                else 
                {
                    goto Thread_Out;
                }

Thread_Found:
                if (!GetClientRect(pThreadBlockInfo->hwndThreadWindow,
                                  &pThreadBlockInfo->rcClient))
                    MessageBox(ghwndMain, "Failed in GetClientRect!", "Error", MB_OK);
Thread_Out:
                LocalUnlock(hHead);

                return DefMDIChildProc(hwnd, message, wParam, lParam);
            } 
            return DefMDIChildProc(hwnd, message, wParam, lParam);
        }
        case WM_SETFOCUS:
            ghwndCurrent = hwnd;
            break;

        case WM_CLOSE: 
        {
            PTHREADBLOCKINFO pThreadBlockInfo;
            PNODE            pHead;
            HANDLE           hHead, hTmp;

            // now find match
            hHead = (HANDLE) GetWindowLong(ghwndMain, 0);
            if (hHead) 
            {
                if ((pHead = (PNODE)LocalLock(hHead))==NULL)
                    MessageBox(ghwndMain, "Failed in LocalLock!", "Error", MB_OK);

                while ((pHead->ThreadWindow.hwndThreadWindow != hwnd) &&
                       (pHead->hNext != NULL)) 
                {
                    hTmp = hHead;
                    hHead = pHead->hNext;
                    LocalUnlock(hTmp);

                    if ((pHead = (PNODE) LocalLock(hHead))==NULL)
                        MessageBox(ghwndMain, "Failed in LocalLock!", "Error", MB_OK);
                }
                if (pHead->ThreadWindow.hwndThreadWindow == hwnd) 
                {
                    pThreadBlockInfo = &pHead->ThreadWindow;
                    goto Thread_Found1;
                } 
                else 
                {
                    MessageBox(ghwndMain, "Trouble - Can't find the thread node!", "Error", MB_OK);
                    goto Thread_Out1;
                }

Thread_Found1:
                pThreadBlockInfo->bKillThrd = TRUE;
                nRc = TtsUninitialize(pThreadBlockInfo->hTtsInst);
                if (nRc!= R_SUCCESSFUL)
                {
                    MessageBox(NULL,"Uninitialize TTS instance failed!",NULL,MB_OK); /* clarify error */
                    PostMessage(ghwndMain,WM_CLOSE,0,0);
                    break;
                }

                CloseHandle(pThreadBlockInfo->hThread);
                
                nThreadCnt--;
                if (nThreadCnt==0)
                {
                    EnableMenuItem(hMenu, IDM_T_SPEAK, MF_GRAYED);
                }

Thread_Out1:
                LocalUnlock(hHead);

            } 
            else 
            {
                MessageBox(ghwndMain, "Can't GetWindowLong(ghwndMain,0) !", "Error", MB_OK);
            }
            return DefMDIChildProc(hwnd, message, wParam, lParam);
        }

        case WM_DESTROY:
            return 0L;

        default:
            return DefMDIChildProc(hwnd, message, wParam, lParam);
    }

}


/*                                                              */        
/*  Thread Handling Function                                    */        
/*                                                              */        
                
long StartThread(PTHREADBLOCKINFO pThreadBlockInfo)
{
    HTTSINSTANCE    hTtsInst;
    TTSSYSTEMINFO   gSysInfo;
    int             nTaskNo;
    int             nEngineId;
    char            szTextStr[1024];
    int             nRc=0, i;
    
        /* Initialize the TTS instance */
    nRc  = TtsInitialize(&hTtsInst, pThreadBlockInfo->hThread, &gSysInfo);    
    if (nRc!= R_SUCCESSFUL)
    {
        MessageBox(NULL,"Initialize TTS instance failed!",NULL,MB_OK); /* clarify error */
        PostMessage(ghwndMain,WM_CLOSE,0,0);
    }

    for(i=0;i<gSysInfo.nMaxEngines;i++)
    {
        /* Find the "American English" TTS engine	*/
        if(strcmp(gSysInfo.mTtsEg[i].szLanguage, "American English") == 0)
        {
            nEngineId = gSysInfo.mTtsEg[i].nEngineId;
            break;	
        }	
    }	
    pThreadBlockInfo->hTtsInst = hTtsInst;
        /* Select the "American English" TTS engine	*/
    TtsSelectEngine(hTtsInst, nEngineId, TRUE);
    wsprintf(szTextStr, "The number of the thread handle is %d.", (int)pThreadBlockInfo->hThread);
    TtsSetSystemMode(hTtsInst, M_PUTTEXTINQUEUE);
        /* Play a text indicating the thread's number	*/
    nRc = TtsSpeakText(hTtsInst, 1, szTextStr, strlen(szTextStr),
                       M_ANSI_TEXT, &nTaskNo);	
    if(nRc!= R_SUCCESSFUL)
    {
        MessageBox(NULL,"Speak TTS text failed!",NULL, MB_OK);
        PostMessage(ghwndMain,WM_CLOSE,0,0);
    }
    return 0;    
}


/*
 *  A dialog function to display TTS SDK product information
 */
BOOL FAR PASCAL About(HWND hWndDlg, WORD Message, WORD wParam, LONG lParam)
{
    HTTSINSTANCE    hTtsInst;
    TTSSYSTEMINFO   gSysInfo;
    int             nEngineId;
    int             nRc=0, i;

    switch(Message)
    {
        case WM_INITDIALOG:
        {
            char            szDummy[20];  
            int             nRc;
            TTSENGINEINFO   mEngine;
            TTSIDENTITY     mIden;
                /* Initialize the TTS instance */	
            nRc = TtsInitialize(&hTtsInst, NULL, &gSysInfo);
            if (nRc!= R_SUCCESSFUL)
            {
                MessageBox(NULL,"Initialize TTS instance failed!",NULL,MB_OK); /* clarify error */
                PostMessage(ghwndMain,WM_CLOSE,0,0);
                break;
            }

            for(i=0;i<gSysInfo.nMaxEngines;i++)
            {
                /* Find the "American English" TTS engine	*/
                if(strcmp(gSysInfo.mTtsEg[i].szLanguage, "American English") == 0)
                {
                    nEngineId = gSysInfo.mTtsEg[i].nEngineId;
                    break;	
                }	
            }	
            TtsSelectEngine(hTtsInst, nEngineId, TRUE);
            TtsGetEngineInfo(hTtsInst, nEngineId, &mEngine);
            SendDlgItemMessage(hWndDlg, ID_LANGUAGE, WM_SETTEXT, 0, (LPARAM) (LPSTR) mEngine.szLanguage);
            SendDlgItemMessage(hWndDlg, ID_COUNTRY, WM_SETTEXT, 0, (LPARAM) (LPSTR) mEngine.szLocation);
            SendDlgItemMessage(hWndDlg, ID_CODING, WM_SETTEXT, 0, (LPARAM) (LPSTR) mEngine.szCodingScheme);
            SendDlgItemMessage(hWndDlg, ID_PRODUCTVERSION, WM_SETTEXT, 0, (LPARAM) (LPSTR) mEngine.szVersion);
            sprintf(szDummy,"%i",mEngine.nSpeaker);
            SendDlgItemMessage(hWndDlg, ID_NUMBER, WM_SETTEXT, 0, (LPARAM) (LPSTR) szDummy);
                /* Get TTS product information	*/
            TtsGetIdentity(&mIden);
            SendDlgItemMessage(hWndDlg, ID_COPYRIGHT, WM_SETTEXT, 0, (LPARAM) (LPSTR) mIden.szCopyright);
            SendDlgItemMessage(hWndDlg, ID_MANUFACTURER, WM_SETTEXT, 0, (LPARAM) (LPSTR) mIden.szManufacturer);
        }                           
        break;
        
        case WM_CLOSE:
            PostMessage(hWndDlg, WM_COMMAND, IDCANCEL, 0L);
            break;

        case WM_COMMAND:
            switch(wParam)
            {
                case IDOK:
                        /* uninitialize TTS instance    */
                    TtsUninitialize(hTtsInst);
                    EndDialog(hWndDlg, TRUE);
                    break;
                case IDCANCEL:
                    EndDialog(hWndDlg, FALSE);
                    break;
            }
            break;

        default:
            return FALSE;
    }
    return TRUE;
}


int  WINAPI InputAndPlayDlg( HWND hWnd,UINT Message,WPARAM wParam,LPARAM lParam)
{
    int     nTaskNo;
    char    szCaption[100];
    int     nRc;
     /*To avoid warnings.*/
    lParam=lParam;

    switch(Message)
    {
        case WM_INITDIALOG: 
        {
            PNODE            pHead;
            HANDLE           hHead, hTmp;
            int Len = GetWindowTextLength(GetDlgItem(hWnd,ID_PLAYTEXT)) +1;

            // now find match
            hHead = (HANDLE) GetWindowLong(ghwndMain, 0);
            if (hHead) 
            {
                if ((pHead = (PNODE)LocalLock(hHead))==NULL)
                    MessageBox(ghwndMain, "Failed in LocalLock!", "Error", MB_OK);
        
                while ((pHead->ThreadWindow.hwndThreadWindow != ghwndCurrent) &&
                        (pHead->hNext != NULL)) 
                {
                    hTmp = hHead;
                    hHead = pHead->hNext;
                    LocalUnlock(hTmp);

                    if ((pHead = (PNODE) LocalLock(hHead))==NULL)
                        MessageBox(ghwndMain, "Failed in LocalLock!", "Error", MB_OK);
                }
                if (pHead->ThreadWindow.hwndThreadWindow == ghwndCurrent) 
                {
                    hTtsInst = pHead->ThreadWindow.hTtsInst;
                }
                wsprintf(szCaption, "Speech Control : %s", pHead->ThreadWindow.CaptionBarText);
                SetWindowText(hWnd, szCaption);
            }
        }
        break;
        
        case WM_CLOSE:
        {
            PostMessage(hWnd, WM_COMMAND, IDOK, 0L);
            break;
        }
        case WM_COMMAND:
        {
            switch (wParam)
            {
                case IDOK:
                {
                    EndDialog(hWnd,R_SUCCESSFUL);
                    break;
                }
                case ID_PLAY:
                {
                    PNODE   pHead;
                    HANDLE  hHead, hTmp;
                    int Len = GetWindowTextLength(GetDlgItem(hWnd,ID_PLAYTEXT)) +1;
                    LPSTR   lpszPlayStr;

                    // now find match
                    hHead = (HANDLE) GetWindowLong(ghwndMain, 0);
                    if (hHead) 
                    {
                        if ((pHead = (PNODE)LocalLock(hHead))==NULL)
                            MessageBox(ghwndMain, "Failed in LocalLock!", "Error", MB_OK);
            
                        while ((pHead->ThreadWindow.hwndThreadWindow != ghwndCurrent) &&
                                    (pHead->hNext != NULL)) 
                        {
                            hTmp = hHead;
                            hHead = pHead->hNext;
                            LocalUnlock(hTmp);

                            if ((pHead = (PNODE) LocalLock(hHead))==NULL)
                                MessageBox(ghwndMain, "Failed in LocalLock!", "Error", MB_OK);
                        }
                        if (pHead->ThreadWindow.hwndThreadWindow == ghwndCurrent) 
                        {
                            hTtsInst = pHead->ThreadWindow.hTtsInst;
                        }
                    }
                    if (Len == 0) break;
                    lpszPlayStr = (LPSTR) malloc(Len);
                    GetWindowText(GetDlgItem(hWnd,ID_PLAYTEXT),lpszPlayStr,Len);
                    EnableWindow(GetDlgItem(hWnd,ID_PLAY),FALSE);
                    EnableWindow(GetDlgItem(hWnd,ID_PAUSE),TRUE);
                    EnableWindow(GetDlgItem(hWnd,ID_STOP),TRUE);
                    EnableWindow(GetDlgItem(hWnd,ID_RESUME),FALSE);
                    EnableWindow(GetDlgItem(hWnd,IDOK),FALSE);
                        /* Play an input text	*/
                    nRc = TtsSpeakText(hTtsInst, 1, lpszPlayStr, 
                                            strlen(lpszPlayStr),M_ANSI_TEXT, &nTaskNo);
                    free(lpszPlayStr);
                    if (nRc!= R_SUCCESSFUL)
                    {
                        MessageBox(NULL,"Speak TTS text failed!",NULL,MB_OK); /* clarify error */
                        PostMessage(hWnd,WM_CLOSE,0,0); /* This message also stops the TTS system. */
                    }
                    EnableWindow(GetDlgItem(hWnd,ID_STOP),FALSE);
                    EnableWindow(GetDlgItem(hWnd,ID_PLAY),TRUE);
                    EnableWindow(GetDlgItem(hWnd,ID_PAUSE),FALSE);
                    EnableWindow(GetDlgItem(hWnd,ID_RESUME),FALSE);
                    EnableWindow(GetDlgItem(hWnd,IDOK),TRUE);
                    break;
                }
                case ID_PAUSE:
                {
                    /* Pause playback	*/
                    nRc = TtsPauseSpeech(hTtsInst, 1);
                    if (nRc!= R_SUCCESSFUL)
                    {
                        MessageBox(NULL,"Pause TTS text failed!",NULL,MB_OK); /* clarify error */
                        PostMessage(hWnd,WM_CLOSE,0,0); /* This message also stops the TTS system. */
                    }
                    EnableWindow(GetDlgItem(hWnd,ID_PAUSE),FALSE);
                    EnableWindow(GetDlgItem(hWnd,ID_RESUME),TRUE);
                    break;
                }
                case ID_RESUME:
                {
                    /* Resume paused playback	*/
                    nRc = TtsResumeSpeech(hTtsInst, 1);
                    if (nRc!= R_SUCCESSFUL)
                    {
                        MessageBox(NULL,"Resume TTS text failed!",NULL,MB_OK); /* clarify error */
                        PostMessage(hWnd,WM_CLOSE,0,0); /* This message also stops the TTS system. */
                    }
                    EnableWindow(GetDlgItem(hWnd,ID_RESUME),FALSE);
                    EnableWindow(GetDlgItem(hWnd,ID_PAUSE),TRUE);
                    break;
                }
                case ID_STOP:
                {
                    /* Stop playing a text	*/
                    nRc = TtsStopSpeech(hTtsInst, 1);
                    if (nRc!= R_SUCCESSFUL)
                    {
                        MessageBox(NULL,"Stop TTS text failed!",NULL,MB_OK); /* clarify error */
                        PostMessage(hWnd,WM_CLOSE,0,0); /* This message also stops the TTS system. */
                    }
                    break;
                }
                default: return FALSE;
            }
            break;
        }
        default: return FALSE;
    }
    return TRUE;
}


int InputAndPlayText(HWND hWnd)
{
    DLGPROC dlgProc = (DLGPROC) MakeProcInstance(InputAndPlayDlg,ghInst);
    return DialogBox(ghInst,"DIALOG_PLAY",hWnd,dlgProc);
}
