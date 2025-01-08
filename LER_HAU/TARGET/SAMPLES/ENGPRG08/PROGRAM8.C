/*
Product  :  Text-To-Speech Software Development Kit
Version  :  4.00
Program  :  PROGRAM8.c
Purpose  :  Program8 illustrates how to use personal dictionaries. As
            in Program7, it offers the same speech control dialog box
            with an edit field to speak, pause, resume and stop the TTS
            speech generator randomly. Next to that, this program allows
            to load, unload, enable and disable one or more personal
            dictionaries.
Comments :  Comments to the source code are given as they pertain to
            the usage of TTS.

Copyright (c) 1996, Lernout & Hauspie Speech Products.
All rights reserved.
*/

#include "program8.h"
#include <stdio.h>
#include <stdlib.h>
#include <commdlg.h>    /* needed for the open file instruction */
#include "ttssdk32.h"   /* Include this header so all TTS routines are known. */

#define FREQUENCY     M_FREQ_11KHZ       /* Use sampling rate of 11 kHz. */

HTTSINSTANCE    hTtsInst;
TTSSYSTEMINFO   gSysInfo;
int             nTaskNo;
int             nLoadedDct;
int             nEngineId;
char            gszString[128];
char            gszAppName[20];
HINSTANCE       ghInst;
HWND            ghWndMain;


#define MAXDICT 10
#define ALL_DICTS 11

typedef  struct
{                         
    HUSRDICT    hDct;
    char        szDictName[260];
    BOOL        bEnabled;
} DICTNAME;

DICTNAME    Dictnames[MAXDICT];

int  CALLBACK InputAndPlayDlg(HWND hWnd,UINT Message,WPARAM wParam,LPARAM lParam)
{
    int nRc;
    lParam=lParam;/*To avoid warning*/

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
            }
            break;
        }
        default: return FALSE;
    }
    return TRUE;
}


int InputAndPlayText( HWND hWnd)
{
    DLGPROC dlgProc = (DLGPROC) MakeProcInstance(InputAndPlayDlg,ghInst);
    return DialogBox(ghInst,"DIALOG_PLAY",hWnd,dlgProc);
}


int  CALLBACK SelectDictDlg( HWND hWnd,UINT Message,WPARAM wParam,LPARAM lParam)
{
    static int FAR * lphDict;
    int nIndex;

    switch(Message)
    {
        case WM_INITDIALOG:
        {
            int Dict;
            lphDict = ( int FAR * ) lParam;
            for (Dict = 0 ; Dict < MAXDICT ; Dict ++)
            {
                if ( (Dictnames[Dict].szDictName[0]!= 0) && ( (*lphDict==ALL_DICTS) || (Dictnames[Dict].bEnabled == *lphDict) ) )
                {
                    WORD nRc = (WORD) SendDlgItemMessage(hWnd,ID_LISTBOX,LB_ADDSTRING,0,(LPARAM)(LPSTR)Dictnames[Dict].szDictName);
                    SendDlgItemMessage(hWnd,ID_LISTBOX,LB_SETITEMDATA,nRc,MAKELONG(Dict,0));
                }
            }
            break;
        }
        case WM_CLOSE:
        {
            PostMessage(hWnd, WM_COMMAND, IDCANCEL, 0L);
            break;
        }
        case WM_COMMAND:
        {
            switch ( wParam)
            {
                case IDOK:
                {
                    LRESULT nRc =  SendDlgItemMessage(hWnd,ID_LISTBOX,LB_GETCURSEL,0,0);
                    if (nRc == LB_ERR) break;
                    nIndex = LOWORD(SendDlgItemMessage(hWnd,ID_LISTBOX,LB_GETITEMDATA,(WORD)nRc,0));
                    *lphDict = nIndex;
                    EndDialog(hWnd,R_SUCCESSFUL);
                    break;
                }
                case IDCANCEL:
                {
                    EndDialog(hWnd,-1);
                    break;
                }
            }
            break;
        }
        default: return FALSE;
    }
    return TRUE;
}


int GetLoadedDict( HWND hWnd, BOOL bFlag ,int FAR *lphDct)
{
    DLGPROC dlgProc = (DLGPROC) MakeProcInstance(SelectDictDlg,ghInst);
    *lphDct = bFlag;
    return DialogBoxParam(ghInst,"DIALOG_DICTIONARY",hWnd,dlgProc,(LPARAM)lphDct);
}


int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
    MSG     msg;

    strcpy(gszAppName, "PROGRAM");
    ghInst = hInstance;
    lpszCmdLine=lpszCmdLine;
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
                     gszAppName,"L&H TTS Sample Program 8",WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_THICKFRAME|WS_CLIPCHILDREN|WS_OVERLAPPED,
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


LONG FAR PASCAL WndProc(HWND hWnd, WORD Message, WORD wParam, LONG lParam)
{
    int         nRc=0, i;
    HUSRDICT    hUDct;
    char        szTextStr[200];
    int         hDct;

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
                case IDM_DICTIONARY_LOAD:
                {
                    char            CustomFilter[60]="";
                    char            szFileName[128]="";
                    char            szFileBase[260]="";
                    OPENFILENAME    OpenFileName;

                    OpenFileName.lStructSize        =sizeof(OPENFILENAME);
                    OpenFileName.hwndOwner          =hWnd;
                    OpenFileName.hInstance          =ghInst;
                                        OpenFileName.lpstrFilter        ="Dictionary Files (*.dcb)\0*.dcb\0All Files (*.*)\0*.*\0\0";
                    OpenFileName.lpstrCustomFilter  =CustomFilter;
                    OpenFileName.nMaxCustFilter     =sizeof(CustomFilter);
                    OpenFileName.nFilterIndex       =1;
                    OpenFileName.lpstrFile          =szFileName;
                    OpenFileName.nMaxFile           =sizeof(szFileName);
                    OpenFileName.lpstrFileTitle     =szFileBase;
                    OpenFileName.nMaxFileTitle      =sizeof(szFileBase);
                    OpenFileName.lpstrInitialDir    =NULL;
                    OpenFileName.lpstrTitle         ="Load Personal Dictionary";
                    OpenFileName.Flags              =OFN_FILEMUSTEXIST;
                    OpenFileName.lpstrDefExt        =NULL;
                    OpenFileName.lpfnHook           =NULL;
                    OpenFileName.lpTemplateName     =NULL;
                    if (GetOpenFileName(&OpenFileName) !=  0)
                    {
                        if (nLoadedDct >= MAXDICT )
                        {
                            MessageBox(NULL,"This sample program allows only 10 loaded dictionaries!",NULL,MB_OK); /* clarify error */
                            break;
                        }
                            /* Load selected dictionary	*/
                        nRc=TtsLoadUsrDict(hTtsInst, &hUDct, szFileName);
                        if (nRc != R_SUCCESSFUL)
                        {
                            MessageBox(NULL,"Load dictionary failed!",NULL,MB_OK); /* clarify error */
                            PostMessage(hWnd,WM_CLOSE,0,0);
                        }
                        else
                        {
                            nLoadedDct++;                                
                            for(i=0;i<MAXDICT;i++)
                            {
                                if(!Dictnames[i].bEnabled)
                                {
                                    strcpy(Dictnames[i].szDictName,szFileBase);
                                    Dictnames[i].bEnabled=TRUE;
                                    Dictnames[i].hDct = hUDct;
                                    break;
                                }
                            }
                        }
                    }
                    break;
                }
                case IDM_DICTIONARY_UNLOAD:
                {
                    nRc = GetLoadedDict(hWnd,ALL_DICTS,&hDct);
                    if (nRc!= R_SUCCESSFUL) break;

                        /* Unload selected dictionary	*/
                    nRc = TtsUnloadUsrDict(hTtsInst, Dictnames[hDct].hDct);
                    if (nRc!= R_SUCCESSFUL)
                    {
                        MessageBox(NULL,"Unload dictionary failed!",NULL,MB_OK); /* clarify error */
                        PostMessage(hWnd,WM_CLOSE,0,0);
                        break;
                    }
                    nLoadedDct--;
                    strcpy(Dictnames[hDct].szDictName,"\0");
                    Dictnames[hDct].bEnabled=FALSE;
                    Dictnames[hDct].hDct = 0;
                    break;
                }
                case IDM_DICTIONARY_ENABLE:
                {
                    nRc =  GetLoadedDict(hWnd,FALSE,&hDct);
                    if (nRc!= R_SUCCESSFUL) break;

                        /* Enable a disbaled dictionary	*/
                    nRc = TtsEnableUsrDict(hTtsInst, (HUSRDICT)Dictnames[hDct].hDct,TRUE);
                    if (nRc!= R_SUCCESSFUL)
                    {
                        MessageBox(NULL,"Enable dictionary failed!",NULL,MB_OK); /* clarify error */
                        PostMessage(hWnd,WM_CLOSE,0,0);
                        break;
                    }
                    Dictnames[hDct].bEnabled=TRUE;
                    break;
                }
                case IDM_DICTIONARY_DISABLE:
                {
                    nRc = GetLoadedDict(hWnd,TRUE,&hDct);
                    if (nRc!= R_SUCCESSFUL) break;
                        /* Disable selected dictionary	*/
                    nRc = TtsEnableUsrDict(hTtsInst, (HUSRDICT)Dictnames[hDct].hDct,FALSE);
                    if (nRc!= R_SUCCESSFUL)
                    {
                        MessageBox(NULL,"Disable dictionary failed!",NULL,MB_OK); /* clarify error */
                        PostMessage(hWnd,WM_CLOSE,0,0);
                        break;
                    }
                    Dictnames[hDct].bEnabled=FALSE;
                    break;
                }
                case IDM_DICTIONARY_UNLOADALL:
                {
                        /* Unload all loaded dictionary	*/
                    nRc = TtsUnloadAllUsrDict(hTtsInst);
                    if (nRc!= R_SUCCESSFUL)
                    {
                        MessageBox(NULL,"Unload all dictionaries failed!",NULL,MB_OK); /* clarify error */
                        PostMessage(hWnd,WM_CLOSE,0,0);
                        break;
                    }
                    for (hDct=0; hDct < MAXDICT; hDct++)
                    {
                        Dictnames[hDct].bEnabled=FALSE;
                        strcpy(Dictnames[hDct].szDictName, "\0");
                        Dictnames[hDct].hDct = 0;
                    }
                    nLoadedDct = 0;
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
                default:
                    return DefWindowProc(hWnd, Message, wParam, lParam);

            }

            break;
        }
        case WM_CREATE:   /* START the TTS system when the window is created. */
        {
            nLoadedDct = 0;
                /* Initialize the TTS instance */
            nRc  = TtsInitialize(&hTtsInst, GetCurrentThread(), &gSysInfo);
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
            strcpy(szTextStr, "LHS. This is sample program 8.");
            nRc = TtsSpeakText(hTtsInst, 1, szTextStr, strlen(szTextStr),
                               M_ANSI_TEXT, &nTaskNo);
            if (nRc!= R_SUCCESSFUL)
            {
                MessageBox(NULL,"Speak TTS text failed!",NULL,MB_OK); /* clarify error */
                PostMessage(hWnd,WM_CLOSE,0,0); /* This message also stops the TTS system. */
            }
            break;      /* End of WM_CREATE */
        }
        case WM_CLOSE:  /* Close the window; Uninitialize the TTS instance */
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
            if (hWnd == ghWndMain)
                PostQuitMessage(0);
            break;
        }

        default: return DefWindowProc(hWnd, Message, wParam, lParam);
    }
    return 0L;
}


BOOL FAR PASCAL About(HWND hWndDlg, WORD Message, WORD wParam, LONG lParam)
{
    lParam=lParam;/*To avoid warning*/
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
        default: return FALSE;
    }
    return TRUE;
}
