/*
Product  :  Text-To-Speech Software Development Kit
Version  :  4.00
Program  :  PROGRAM6.c
Purpose  :  This program illustrates the use of more than one language
            in the same application, e.g. American English and German.
            The program will only run if you have both languages
            available. You can run the program with a different combination
            of languages by changing a few lines in the source code
            discussed below, and by recompiling. Start the program and
            choose TTS. A text will be played back in the currently
            selected language. Choose About (Help menu) for more
            information on the selected language. Use the Language menu to
            select the language you want to use, provided it has been
            installed.
Comments :  

Copyright (c) 1996, Lernout & Hauspie Speech Products.
All rights reserved.
*/

#include "PROGRAM6.h"
#include <stdio.h>
#include "ttssdk32.h"   /* Include this header so all TTS routines are known. */

HTTSINSTANCE    hTtsInst;
TTSSYSTEMINFO	gSysInfo;
int             nTaskNo;
int             nEngineId;
char            gszString[128];
char            gszAppName[20];
HINSTANCE       ghInst;
HWND            ghWndMain;
HANDLE          ghLibrary;   /* Library with the TTS routines */

char            gszSentence[50];      
static	int     nLangNo = 0;
BOOL            bAmEngAvail = FALSE;
BOOL            bGermAvail = FALSE;

/*
 *  Set engine
 */
short nLoadRoutines(int nLanguage)
{
    short nRc;
    int	i;

    /* 	To set the language-specific information, a switch is used. This
        particular example only uses the American English and German TTS
        systems, but the procedure is analogous for all other languages
        available as TTS32 SDKs. */

    switch (nLanguage)
    {
        case ENG:
            for(i=0;i<gSysInfo.nMaxEngines;i++)
            {
                /* Find the "American English" TTS engine	*/
                if(strcmp(gSysInfo.mTtsEg[i].szLanguage, "American English") == 0)
                {
                    nEngineId = gSysInfo.mTtsEg[i].nEngineId;
                    break;	
                }	
            }	
                /* Select the "American English" TTS engine	*/
            nRc=TtsSelectEngine(hTtsInst, nEngineId, TRUE);
            if(nRc == R_SUCCESSFUL)
            {
                bAmEngAvail = TRUE;
                strcpy(gszSentence,"This is the American English TTS system.");
            }
            else
                return -1;
            break;
        case GER:
            for(i=0;i<gSysInfo.nMaxEngines;i++)
            {
                /* Find the "German" TTS engine	*/
                if(strcmp(gSysInfo.mTtsEg[i].szLanguage, "German") == 0)
                {
                    nEngineId = gSysInfo.mTtsEg[i].nEngineId;
                    break;	
                }	
            }	
            /* Select the "German" TTS engine	*/
            nRc=TtsSelectEngine(hTtsInst, nEngineId, TRUE);
            if(nRc == R_SUCCESSFUL)
            {
                bGermAvail = TRUE;
                strcpy(gszSentence,"Dies ist das deutsche Text-nach-Sprach System.");
            }
            else
                return -1;
            break;
        default:
            MessageBox(ghWndMain,"Invalid Language has beend found!",NULL,MB_OK);
            return -1;
    }
    return nRc;
}


/*
 *  Main Windows Function
 */
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
    MSG     msg;

    strcpy(gszAppName, "PROGRAM6");
    ghInst = hInstance;
    if(!hPrevInstance)
    {
        WNDCLASS   wndclass;
        memset(&wndclass, 0x00, sizeof(WNDCLASS));

        wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNWINDOW;
        wndclass.lpfnWndProc = (WNDPROC) WndProc;
        wndclass.cbClsExtra = 0;
        wndclass.cbWndExtra = 0;
        wndclass.hInstance = ghInst;
        wndclass.hIcon = LoadIcon(ghInst, "PROGRAMICON");
        wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
        wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
        wndclass.lpszMenuName = gszAppName;
        wndclass.lpszClassName = gszAppName;
        if(!RegisterClass(&wndclass))
        {
            LoadString(ghInst, IDS_ERR_REGISTER_CLASS, gszString, sizeof(gszString));
            MessageBox(NULL, gszString, NULL, MB_ICONEXCLAMATION);
            return -1;
        }
    }

    ghWndMain = CreateWindow(
                     gszAppName,"L&H TTS Sample Program 6",WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_THICKFRAME|WS_CLIPCHILDREN|WS_OVERLAPPED,
                     CW_USEDEFAULT,0,CW_USEDEFAULT,0,NULL,NULL,ghInst,NULL);

    if(ghWndMain == NULL)
    {
        LoadString(ghInst, IDS_ERR_CREATE_WINDOW, gszString, sizeof(gszString));
        MessageBox(NULL, gszString, NULL, MB_ICONEXCLAMATION);
        return IDS_ERR_CREATE_WINDOW;
    }

    ShowWindow(ghWndMain, nCmdShow);
    while(GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnregisterClass(gszAppName, ghInst);
    return msg.wParam;
}


/*
 *  Windows Message Handling Function
 */
LONG FAR PASCAL WndProc(HWND hWnd, WORD Message, WORD wParam, LONG lParam)
{
    int     nRc=0;
    HMENU   hMenu;

    switch (Message)
    {
        case WM_COMMAND:
            switch (wParam)
            {
                case IDM_F_EXIT:
                    PostMessage(hWnd,WM_CLOSE,0,0);
                    break;
                case ENG:
                case GER:
                case FRE:
                case SPA:
                {
                    HMENU hMenu = GetMenu(hWnd);
                    CheckMenuItem(hMenu,ENG,MF_BYCOMMAND | MF_UNCHECKED);
                    CheckMenuItem(hMenu,GER,MF_BYCOMMAND | MF_UNCHECKED);
                    CheckMenuItem(hMenu,wParam,MF_BYCOMMAND | MF_CHECKED);

                    nRc = nLoadRoutines(wParam);
                    if (nRc != R_SUCCESSFUL) 
                    {
                        MessageBox(hWnd, "Selected Engine is not available!", NULL, MB_OK);
                        PostMessage(hWnd,WM_CLOSE,0,0);
                    }
                }
                break;
                
                case IDM_T_SPEAK:
                    hMenu = GetMenu(hWnd);
                    EnableMenuItem(hMenu, IDM_F_EXIT, MF_GRAYED);
                    EnableMenuItem(hMenu, ENG, MF_GRAYED);
                    EnableMenuItem(hMenu, GER, MF_GRAYED);
                    EnableMenuItem(hMenu, IDM_T_SPEAK, MF_GRAYED);
                    EnableMenuItem(hMenu, IDM_H_ABOUT, MF_GRAYED);
                    nRc = TtsSpeakText(hTtsInst, 1, gszSentence, strlen(gszSentence),
                                       M_ANSI_TEXT, &nTaskNo);
                    if (nRc!= R_SUCCESSFUL)
                    {
                        MessageBox(NULL,"Speak TTS text failed!",NULL,MB_OK); /* clarify error */
                        PostMessage(hWnd,WM_CLOSE,0,0);
                    }
                    EnableMenuItem(hMenu, IDM_F_EXIT, MF_ENABLED);
                    if(bAmEngAvail == TRUE)
                        EnableMenuItem(hMenu, ENG, MF_ENABLED);
                    if(bGermAvail == TRUE)
                        EnableMenuItem(hMenu, GER, MF_ENABLED);
                    EnableMenuItem(hMenu, IDM_T_SPEAK, MF_ENABLED);
                    EnableMenuItem(hMenu, IDM_H_ABOUT, MF_ENABLED);
                    break;
                case IDM_H_ABOUT:
                {
                    FARPROC lpfnDIALOGMsgProc;

                    lpfnDIALOGMsgProc = MakeProcInstance((FARPROC)About, ghInst);
                    nRc = DialogBox(ghInst, MAKEINTRESOURCE(100), hWnd, lpfnDIALOGMsgProc);
                    FreeProcInstance(lpfnDIALOGMsgProc);
                }
                break;

                default:
                    return DefWindowProc(hWnd, Message, wParam, lParam);
            }
            break;

        case WM_CREATE:   /* Initialize the TTS instance when the window is created. */
            hMenu = GetMenu(hWnd);
            nRc  = TtsInitialize(&hTtsInst, GetCurrentThread(), &gSysInfo);
            if (nRc!= R_SUCCESSFUL)
            {
                MessageBox(NULL,"Initialize TTS instance failed!",NULL,MB_OK);
                PostMessage(hWnd,WM_CLOSE,0,0);
                return -1;
            }
            if (nLoadRoutines(GER) != R_SUCCESSFUL)
            {
                MessageBox(NULL,"German language is not found!",NULL,MB_OK);
                EnableMenuItem(hMenu, GER, MF_GRAYED);
            }
            else
                CheckMenuItem(hMenu,GER,MF_BYCOMMAND | MF_CHECKED);

            if (nLoadRoutines(ENG) != R_SUCCESSFUL)
            {
                MessageBox(NULL,"American English language is not found!",NULL,MB_OK);
                EnableMenuItem(hMenu, ENG, MF_GRAYED);
            }
            else
            {
                CheckMenuItem(hMenu,GER,MF_BYCOMMAND | MF_UNCHECKED);
                CheckMenuItem(hMenu,ENG,MF_BYCOMMAND | MF_CHECKED);
            }
            break;      /* End of WM_CREATE */

        case WM_CLOSE:  /* Unitialize the TTS instance	*/
            nRc = TtsUninitialize(hTtsInst);
            if (nRc!= R_SUCCESSFUL)
            {
                MessageBox(NULL,"Uninitialize TTS instance failed!",NULL,MB_OK); /* clarify error */
                PostMessage(hWnd,WM_CLOSE,0,0);
            }
            DestroyWindow(hWnd);
            if (hWnd == ghWndMain)
                PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, Message, wParam, lParam);
    }
    return 0L;
}


/*
 *  Dialog Function to display TTS SDK product information
 */
BOOL FAR PASCAL About(HWND hWndDlg, WORD Message, WORD wParam, LONG lParam)
{
    switch(Message)
    {
        case WM_INITDIALOG:
        {
            char            szDummy[20];  
            TTSENGINEINFO   mEngine;
            TTSIDENTITY     mIden;

                /* Get TTS engine information	*/          
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
