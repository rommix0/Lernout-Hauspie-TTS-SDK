/*
Product  :  Text-To-Speech Software Development Kit
Version  :  4.00
Program  :  PROGRAM7.c
Purpose  :  Program7 illustrates the multitasking routines. The program
            offers a speech control dialog box with an edit field.
            You can speak, pause, resume and stop the TTS speech
            generator randomly.
Comments :  Comments to the source code are given as they pertain to
            the usage of TTS.

Copyright (c) 1996, Lernout & Hauspie Speech Products.
All rights reserved.
*/

#include "program7.h"
#include <stdio.h>
#include <stdlib.h>
#include "ttssdk32.h"   /* Include this header so all TTS routines are known. */

#define FREQUENCY     M_FREQ_11KHZ       /* Use sampling rate of 11 kHz. */

HTTSINSTANCE    hTtsInst;
TTSSYSTEMINFO   gSysInfo;
int             nTaskNo;
int             nEngineId;
char            gszString[128];
char            gszAppName[20];
HINSTANCE       ghInst;
HWND            ghWndMain;

/*
 *  Playback Dialog Handling Function
 */
int  CALLBACK InputAndPlayDlg(HWND hWnd,UINT Message,WPARAM wParam,LPARAM lParam)
{
    int nRc;
    /*To avoid warnings.*/
    lParam=lParam;

    switch(Message)
    {
        case WM_INITDIALOG: break;
        case WM_CLOSE:
        {
            PostMessage(hWnd, WM_COMMAND, IDOK, 0L);
            break;
        }
        case WM_COMMAND:
        {
            switch ( wParam)
            {
                case IDOK:
                {
                    EndDialog(hWnd,R_SUCCESSFUL);
                    break;
                }
                case ID_PLAY:
                {
                    int Len = GetWindowTextLength(GetDlgItem(hWnd,ID_PLAYTEXT)) +1;
                    LPSTR lpszPlayStr;
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
                                       strlen(lpszPlayStr), M_ANSI_TEXT, &nTaskNo);
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
                        /* Pause playing a text	*/
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
                default: 
                    return FALSE;
            }
            break;
        }
        default: 
            return FALSE;
    }
    return TRUE;
}


int InputAndPlayText( HWND hWnd)
{
    DLGPROC dlgProc = (DLGPROC) MakeProcInstance(InputAndPlayDlg,ghInst);
    return DialogBox(ghInst,"DIALOG_PLAY",hWnd,dlgProc);
}


/*
 *  Main Windows Function
 */
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
    MSG     msg;

    strcpy(gszAppName, "PROGRAM");
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
                     gszAppName,"L&H TTS Sample Program 7",WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_THICKFRAME|WS_CLIPCHILDREN|WS_OVERLAPPED,
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
    int     nRc=0, i;
    char    szTextStr[200];

    switch (Message)
    {
        case WM_COMMAND:
        {
            switch (wParam)
            {
                case IDM_F_EXIT:
                {
                    PostMessage(hWnd,WM_CLOSE,0,0);
                    break;
                }
                case IDM_TTS_PLAY:
                {
                    nRc =InputAndPlayText(hWnd);
                    if (nRc!= R_SUCCESSFUL)
                    {
                        MessageBox(NULL,"Play TTS text failed!",NULL,MB_OK); /* clarify error */
                        PostMessage(hWnd,WM_CLOSE,0,0);
                    }
                    break;
                }
                case IDM_H_ABOUT:
                {
                    FARPROC lpfnDIALOGMsgProc;

                    lpfnDIALOGMsgProc = MakeProcInstance((FARPROC)About, ghInst);
                    nRc = DialogBox(ghInst, MAKEINTRESOURCE(100), hWnd, lpfnDIALOGMsgProc);
                    FreeProcInstance(lpfnDIALOGMsgProc);
                    break;
                }
                default: return DefWindowProc(hWnd, Message, wParam, lParam);
            }
            break;
        }
        case WM_CREATE:   /* Initialize the TTS instance when the window is created. */
        {
                /* Initialize the TTS instance */
            nRc  = TtsInitialize(&hTtsInst, NULL, &gSysInfo);
            if (nRc!= R_SUCCESSFUL)
            {
                MessageBox(NULL,"Initialize TTS instance failed!",NULL,MB_OK); /* clarify error */
                PostMessage(hWnd,WM_CLOSE,0,0); /* This message also stops the TTS system. */
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
                /* Select the "American English" TTS engine	*/
            nRc=TtsSelectEngine(hTtsInst, nEngineId, TRUE);

            if (nRc!= R_SUCCESSFUL)
            {
                MessageBox(NULL,"Select TTS engine failed!",NULL,MB_OK); /* clarify error */
                PostMessage(hWnd,WM_CLOSE,0,0); /* This message also stops the TTS system. */
                break;
            }
                /* Set frequency to 11.025 kHz	*/
            TtsSetFreq(hTtsInst, FREQUENCY);
                /* Playing an introductory text */
            strcpy(szTextStr, "LHS. This is sample program 7.");
            nRc = TtsSpeakText(hTtsInst, 1, szTextStr, strlen(szTextStr),
                               M_ANSI_TEXT, &nTaskNo);
            if (nRc!= R_SUCCESSFUL)
            {
                MessageBox(NULL,"Speak TTS text failed!",NULL,MB_OK); /* clarify error */
                PostMessage(hWnd,WM_CLOSE,0,0); /* This message also stops the TTS system. */
            }
            break;      /* End of WM_CREATE */
        }
        case WM_CLOSE:  /* Close the window; Uninitialize the TTS instance. */
        {
                /* Play goodbye message */
            strcpy(szTextStr, "Thank you for using the L&H TTS system!");
            nRc = TtsSpeakText(hTtsInst, 1, szTextStr, strlen(szTextStr),
                               M_ANSI_TEXT, &nTaskNo);
                /* Uninitialize the TTS instance */
            nRc = TtsUninitialize(hTtsInst);
            if (nRc!= R_SUCCESSFUL)
            {
                MessageBox(NULL,"Uninitialize TTS instance failed!",NULL,MB_OK); /* clarify error */
            }

            DestroyWindow(hWnd);
            if (hWnd == ghWndMain) PostQuitMessage(0);
            break;
        }
        default: 
            return DefWindowProc(hWnd, Message, wParam, lParam);
    }
    return 0L;
}


/*
 *  Dialog function to display TTS product information
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
            break;
        }
        case WM_CLOSE:
        {
            PostMessage(hWndDlg, WM_COMMAND, IDCANCEL, 0L);
            break;
        }
        case WM_COMMAND:
        {
            switch(wParam)
            {
                case IDOK:
                {
                    EndDialog(hWndDlg, TRUE);
                    break;
                }
                case IDCANCEL:
                {
                    EndDialog(hWndDlg, FALSE);
                    break;
                }
            }
            break;
        }
        default: 
            return FALSE;
    }
    return TRUE;
}
