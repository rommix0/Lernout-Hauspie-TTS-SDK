/*
Product  :  Text-To-Speech Software Development Kit
Version  :  4.00
Program  :  PROGRAM9.c
Purpose  :  Program9 illustrates how you can generate PCM data buffers
            and play them back via a sound device. It uses the same speech
            control dialog box as in Program7, with an additional SPEAK&SAVE
            button. This button allows the TTS system to speak the text and
            create a wave file sequentially.
Comments :  Comments to the source code are given as they pertain to
            the usage of TTS.

Copyright (c) 1996, Lernout & Hauspie Speech Products.
All rights reserved.
*/

#include "program9.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <commdlg.h>    /* needed for the open file instruction */
#include <mmsystem.h>
#include "ttssdk32.h"   /* Include this header so all TTS routines are known. */

#define FREQUENCY     M_FREQ_11KHZ       /* Use sampling rate of 11 kHz. */
#define SAMPLEFREQ    11025              /* Define for wave buffers.*/

typedef struct {
                 char   szRiff[4];
                 long   lFileSize;
                 char   szWave[8];
                 long   lUnknown;
                 short  nFormat;
                 short  nChannels;
                 long   lSamplesPerSec;
                 long   lAvgBytesPerSec;
                 short  lBlockAlign;
                 short  nBits;
                 char   szID3[4];
                 long   lDataSize;
                } WAVEFILEHEADER;

HTTSINSTANCE    hTtsInst;
TTSSYSTEMINFO   gSysInfo;
int             nTaskNo;
int             nLoadedDct;
int             nCurPcmBuf;
int             nEngineId;
char            gszString[128];
char            gszAppName[20];
HINSTANCE       ghInst;
HWND            ghWndMain;

#define MAXDICT             10
#define ALL_DICTS           11
#define SAMPLEBUFFERSIZE    1024  * 11
#define BITSPERBYTE         8

typedef  struct
{
    HUSRDICT	hDct;
    char        szDictName[260];
    BOOL        bEnabled;
} DICTNAME;

DICTNAME    Dictnames[MAXDICT];

int  CALLBACK InputAndPlayDlg( HWND hWnd,UINT Message,WPARAM wParam,LPARAM lParam)
{
    int                 nRc, nDataLen;
    static HWAVEOUT     hWaveOut;
    static DWORD        gLastBuffer;


    lParam=lParam; /*To avoid warning.*/

    switch(Message)
    {
        case WM_INITDIALOG: break;
        case WM_CLOSE:
        {
            PostMessage(hWnd, WM_COMMAND, IDOK, 0L);
            break;
        }
        case WM_DESTROY:
        {
            if (hWaveOut!= NULL)
            {
                waveOutClose(hWaveOut);
            }
            break;
        }
        case MM_WOM_DONE:
        {
            LPWAVEHDR lpWave =(LPWAVEHDR)lParam;
            waveOutUnprepareHeader((HWAVEOUT)wParam,lpWave,sizeof(WAVEHDR));
            if (gLastBuffer == lpWave->dwUser)
            {
                waveOutClose(hWaveOut);
                hWaveOut = NULL;

                EnableWindow(GetDlgItem(hWnd,ID_STOP),FALSE);
                EnableWindow(GetDlgItem(hWnd,ID_PLAY),TRUE);
                EnableWindow(GetDlgItem(hWnd,ID_GENERATE),TRUE);
                EnableWindow(GetDlgItem(hWnd,ID_PAUSE),FALSE);
                EnableWindow(GetDlgItem(hWnd,ID_RESUME),FALSE);
                EnableWindow(GetDlgItem(hWnd,IDOK),TRUE);
            }
            GlobalUnlock((HGLOBAL)LOWORD(lpWave->dwUser));
            GlobalFree((HGLOBAL)LOWORD(lpWave->dwUser));
            GlobalUnlock((HGLOBAL)HIWORD(lpWave->dwUser));
            GlobalFree((HGLOBAL)HIWORD(lpWave->dwUser));
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
                    if (Len == 1) break;
                    lpszPlayStr = (LPSTR) malloc(Len);
                    GetWindowText(GetDlgItem(hWnd,ID_PLAYTEXT),lpszPlayStr,Len);
                    EnableWindow(GetDlgItem(hWnd,ID_PLAY),FALSE);
                    EnableWindow(GetDlgItem(hWnd,ID_PAUSE),TRUE);
                    EnableWindow(GetDlgItem(hWnd,ID_STOP),TRUE);
                    EnableWindow(GetDlgItem(hWnd,ID_RESUME),FALSE);
                    EnableWindow(GetDlgItem(hWnd,IDOK),FALSE);
                    EnableWindow(GetDlgItem(hWnd,ID_GENERATE),FALSE);
                        /* Play an input text	*/
                    nRc = TtsSpeakText(hTtsInst, 1, lpszPlayStr, 
                                       strlen(lpszPlayStr),M_ANSI_TEXT, &nTaskNo);
                    free(lpszPlayStr);
                    if (nRc!= R_SUCCESSFUL)
                    {
                        MessageBox(NULL,"Speak TTS text failed!",NULL,MB_OK);
                        PostMessage(hWnd,WM_CLOSE,0,0); /* This message also stops the TTS system. */
                    }
                    EnableWindow(GetDlgItem(hWnd,ID_STOP),FALSE);
                    EnableWindow(GetDlgItem(hWnd,ID_PLAY),TRUE);
                    EnableWindow(GetDlgItem(hWnd,ID_PAUSE),FALSE);
                    EnableWindow(GetDlgItem(hWnd,ID_RESUME),FALSE);
                    EnableWindow(GetDlgItem(hWnd,IDOK),TRUE);
                    EnableWindow(GetDlgItem(hWnd,ID_GENERATE),TRUE);

                    break;
                }
                case ID_PAUSE:
                {
                        /* Pause playing a text	*/
                    nRc = TtsPauseSpeech(hTtsInst, 1);
                    if (nRc!= R_SUCCESSFUL)
                    {
                        MessageBox(NULL,"Pause TTS text failed!",NULL,MB_OK);
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
                        MessageBox(NULL,"Resume TTS text failed!",NULL,MB_OK);
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
                        MessageBox(NULL,"Stop TTS text failed!",NULL,MB_OK);
                        PostMessage(hWnd,WM_CLOSE,0,0); /* This message also stops the TTS system. */
                    }
                    break;
                }
                case ID_GENERATE:
                {
                    int               Len = GetWindowTextLength(GetDlgItem(hWnd,ID_PLAYTEXT)) +1;
                    LPSTR             lpszPlayStr;
                    HANDLE            hBuffer = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE ,SAMPLEBUFFERSIZE);
                    LPSTR             lpPcmBuf = (LPSTR)GlobalLock(hBuffer);
                    OFSTRUCT          ofStruct;
                    LONG              lDataSize;
                    WAVEFILEHEADER    WaveFileHeader;
                    HANDLE            hHeader = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE ,sizeof(WAVEHDR));
                    LPWAVEHDR         lpWaveHeader = (LPWAVEHDR) GlobalLock(hHeader);
                    PCMWAVEFORMAT     WaveFormat;
                    HFILE             hFile;
                    
                    if (Len == 1) break;
                    lpszPlayStr = (LPSTR) malloc(Len);
                    GetWindowText(GetDlgItem(hWnd,ID_PLAYTEXT),lpszPlayStr,Len);
                    EnableWindow(GetDlgItem(hWnd,ID_GENERATE),FALSE);
                    EnableWindow(GetDlgItem(hWnd,ID_PLAY),FALSE);
                    EnableWindow(GetDlgItem(hWnd,ID_PAUSE),FALSE);
                    EnableWindow(GetDlgItem(hWnd,ID_STOP),FALSE);
                    EnableWindow(GetDlgItem(hWnd,ID_RESUME),FALSE);
                    EnableWindow(GetDlgItem(hWnd,IDOK),FALSE);
                        /* Generate the first PCM buffer    */
                    nRc = TtsGenPcmBuf(hTtsInst, lpszPlayStr, strlen(lpszPlayStr),
                                       M_OUT_16LINEAR, M_ANSI_TEXT, 
                                       SAMPLEBUFFERSIZE , (LPBYTE)lpPcmBuf,
                                       &nDataLen, &nTaskNo);
                    if (nRc < R_SUCCESSFUL)
                    {
                        MessageBox(NULL,"Generate data buffers failed!",NULL,MB_OK);
                        PostMessage(hWnd,WM_CLOSE,0,0);
                        break;
                    }

                          /* Open a file to save the output.*/
                    hFile = OpenFile("TTS.WAV",&ofStruct,OF_CREATE);
                    if (hFile == HFILE_ERROR)
                    {
                        MessageBox(NULL,"Creation of file failed!",NULL,MB_OK);
                        PostMessage(hWnd,WM_CLOSE,0,0);
                        break;
                    }
                          /* Write the empty header and first data block to file.*/
                    _lwrite(hFile,(LPCSTR)&WaveFileHeader,sizeof(WAVEFILEHEADER));
                    _lwrite(hFile,lpPcmBuf,nDataLen);
                    lDataSize = nDataLen;

                          /* Play the header on the wave device.*/
                    WaveFormat.wf.wFormatTag = WAVE_FORMAT_PCM;
                    WaveFormat.wf.nChannels = 1;
                    WaveFormat.wf.nBlockAlign = 2;
                    WaveFormat.wf.nSamplesPerSec = SAMPLEFREQ;
                    WaveFormat.wf.nAvgBytesPerSec = WaveFormat.wf.nSamplesPerSec * WaveFormat.wf.nBlockAlign;
                    WaveFormat.wBitsPerSample = WaveFormat.wf.nBlockAlign * BITSPERBYTE;
                    
                    nRc =  waveOutOpen(&hWaveOut,(UINT)WAVE_MAPPER,(LPWAVEFORMATEX)&WaveFormat,(DWORD)hWnd,0,CALLBACK_WINDOW);
                    if (nRc != R_SUCCESSFUL)
                    {
                        char szError[200];
                        waveOutGetErrorText(nRc,szError,sizeof(szError));
                        hWaveOut = NULL;
                    }

                    lpWaveHeader->lpData = lpPcmBuf;
                    lpWaveHeader->dwBufferLength = lDataSize;
                    lpWaveHeader->dwUser = MAKELONG(hBuffer,hHeader);
                    lpWaveHeader->dwLoops = 1;
                    lpWaveHeader->dwFlags = 0;

                    nRc = waveOutPrepareHeader(hWaveOut, lpWaveHeader, sizeof(WAVEHDR));
                    if (nRc != R_SUCCESSFUL)
                    {
                        char szError[200];
                        waveOutGetErrorText(nRc,szError,sizeof(szError));
                        hWaveOut = NULL;
                    }
                    nRc = waveOutWrite(hWaveOut, lpWaveHeader, sizeof(WAVEHDR));
                    if (nRc != R_SUCCESSFUL)
                    {
                        char szError[200];
                        waveOutGetErrorText(nRc,szError,sizeof(szError));
                        hWaveOut = NULL;
                    }

                    nCurPcmBuf = 0;
                    while  (nRc!= M_PCMGENDONE)
                    {
                        hBuffer = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE ,SAMPLEBUFFERSIZE);
                        lpPcmBuf = (LPSTR)GlobalLock(hBuffer);
                        hHeader = GlobalAlloc(GMEM_MOVEABLE,sizeof(WAVEHDR));
                        lpWaveHeader = (LPWAVEHDR) GlobalLock(hHeader);
                        nCurPcmBuf++;
                            /* Generate the next PCM buffer	*/
                        nRc = TtsGenNextPcmBuf(hTtsInst, SAMPLEBUFFERSIZE, (LPBYTE)lpPcmBuf, 
                                                M_OUT_16LINEAR, &nDataLen, &nTaskNo); 
                        if (nRc == M_PCMGENDONE) break;
                        if  (nRc < R_SUCCESSFUL)
                        {
                            MessageBox(NULL,"Generate data buffers failed!",NULL,MB_OK);
                            PostMessage(hWnd,WM_CLOSE,0,0);
                            break;
                        }
                        _lwrite(hFile,lpPcmBuf,nDataLen);
                        lDataSize += nDataLen;

                        lpWaveHeader->lpData = lpPcmBuf;
                        gLastBuffer = lpWaveHeader->dwUser = MAKELONG(hBuffer,hHeader);
                        lpWaveHeader->dwLoops = 1;	         
                        lpWaveHeader->dwFlags = 0;
                        lpWaveHeader->dwBufferLength = (DWORD)nDataLen;

                        nRc = waveOutPrepareHeader(hWaveOut, lpWaveHeader, sizeof(WAVEHDR));
                        if (nRc != R_SUCCESSFUL)
                        {
                            char szError[200];
                            waveOutGetErrorText(nRc,szError,sizeof(szError));
                            MessageBox(NULL,szError,NULL,MB_OK);
                        }

                        nRc = waveOutWrite(hWaveOut, lpWaveHeader, sizeof(WAVEHDR));
                        if (nRc != R_SUCCESSFUL)
                        {
                            char szError[200];
                            waveOutGetErrorText(nRc,szError,sizeof(szError));
                            MessageBox(NULL,szError,NULL,MB_OK);
                        }
                    }

                          /* Prepare a wave file header. (Note: this is all hard coded)*/
                    strcpy(WaveFileHeader.szRiff,"RIFF");
                    strcpy(WaveFileHeader.szWave,"WAVEfmt ");
                    strcpy(WaveFileHeader.szID3,"data");
                    WaveFileHeader.nFormat          = WaveFormat.wf.wFormatTag;
                    WaveFileHeader.lSamplesPerSec   = WaveFormat.wf.nSamplesPerSec;
                    WaveFileHeader.lUnknown         = 16;
                    WaveFileHeader.nChannels        = WaveFormat.wf.nChannels;
                    WaveFileHeader.lBlockAlign      = WaveFormat.wf.nBlockAlign;
                    WaveFileHeader.nBits            = WaveFormat.wBitsPerSample;
                    WaveFileHeader.lAvgBytesPerSec  = WaveFormat.wf.nAvgBytesPerSec;
                    WaveFileHeader.lFileSize = lDataSize  + sizeof(WAVEFILEHEADER);
                    WaveFileHeader.lDataSize = lDataSize;
                    _llseek(hFile,0,0);
                    _lwrite(hFile,(LPCSTR)&WaveFileHeader,sizeof(WAVEFILEHEADER));
                    _lclose(hFile);
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


int InputAndPlayText( HWND hWnd)
{
    DLGPROC dlgProc = (DLGPROC) MakeProcInstance(InputAndPlayDlg,ghInst);
    return DialogBox(ghInst,"DIALOG_PLAY",hWnd,dlgProc);
}


int  CALLBACK SelectDictDlg( HWND hWnd,UINT Message,WPARAM wParam,LPARAM lParam)
{
    static int FAR * lphDict;

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
                    *lphDict = LOWORD(SendDlgItemMessage(hWnd,ID_LISTBOX,LB_GETITEMDATA,(WORD)nRc,0));
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
        default: 
            return FALSE;
    }
    return TRUE;
}


int GetLoadedDict( HWND hWnd, BOOL bFlag ,int FAR *lphDct)
{
    DLGPROC dlgProc = (DLGPROC) MakeProcInstance(SelectDictDlg,ghInst);
    *lphDct = bFlag;
    return DialogBoxParam(ghInst,"DIALOG_DICTIONARY",hWnd,dlgProc,(LPARAM)lphDct);
}


/*
 *  Main Windows Function
 */
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
    MSG     msg;

    strcpy(gszAppName, "PROGRAM");
    ghInst = hInstance;
    lpszCmdLine = lpszCmdLine; /* To avoid warning.*/
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
                     gszAppName,"L&H TTS Sample Program 9",WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_THICKFRAME|WS_CLIPCHILDREN|WS_OVERLAPPED,
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
    int             nRc=0, i;
    HUSRDICT        hUDct;
    char            szTextStr[200];
    char            CustomFilter[60];
    char            szFileName[128];
    char            szFileBase[260];
    OPENFILENAME    OpenFileName;
    int             hDct;
    FARPROC         lpfnDIALOGMsgProc;

    switch (Message)
    {
        case WM_COMMAND:
            switch (wParam)
            {
                case IDM_F_EXIT:
                    PostMessage(hWnd,WM_CLOSE,0,0);
                    break;
                case IDM_DICTIONARY_LOAD:
                    strcpy(CustomFilter, "");
                    strcpy(szFileName, "");
                    strcpy(szFileBase, "");
                    

                    OpenFileName.lStructSize          =sizeof(OPENFILENAME);
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
                    OpenFileName.lpstrTitle    	  ="Load Personal Dictionary";
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
                case IDM_DICTIONARY_UNLOAD:	                       
                    nRc = GetLoadedDict(hWnd,ALL_DICTS,&hDct);
                    if (nRc!= R_SUCCESSFUL) break;
                        /* Unload selected dictionary	*/
                    nRc = TtsUnloadUsrDict(hTtsInst, (HUSRDICT)Dictnames[hDct].hDct);
                    if (nRc!= R_SUCCESSFUL)
                    {
                        MessageBox(NULL,"Unload dictionary failed!",NULL,MB_OK);
                        PostMessage(hWnd,WM_CLOSE,0,0);
                        break;
                    }
                    nLoadedDct--;
                    strcpy(Dictnames[hDct].szDictName,"\0");
                    Dictnames[hDct].bEnabled=FALSE;
                    Dictnames[hDct].hDct = 0;
                    break;
                case IDM_DICTIONARY_ENABLE:
                    nRc = GetLoadedDict(hWnd,FALSE,&hDct);
                    if (nRc!= R_SUCCESSFUL) break;
                        /* Enable a disabled dictionary	*/
                    nRc = TtsEnableUsrDict(hTtsInst, (HUSRDICT)Dictnames[hDct].hDct,TRUE);
                    if (nRc!= R_SUCCESSFUL)
                    {
                        MessageBox(NULL,"Enable dictionary failed!",NULL,MB_OK);
                        PostMessage(hWnd,WM_CLOSE,0,0);
                        break;
                    }
                    Dictnames[hDct].bEnabled=TRUE;
                    break;
                case IDM_DICTIONARY_DISABLE:
                    nRc = GetLoadedDict(hWnd,TRUE,&hDct);
                    if (nRc!= R_SUCCESSFUL) break;
                        /* Disable a selected dictionary    */
                    nRc = TtsEnableUsrDict(hTtsInst, (HUSRDICT)Dictnames[hDct].hDct,FALSE);
                    if (nRc!= R_SUCCESSFUL)
                    {
                        MessageBox(NULL,"Disable dictionary failed",NULL,MB_OK);
                        PostMessage(hWnd,WM_CLOSE,0,0);
                        break;
                    }
                    Dictnames[hDct].bEnabled=FALSE;
                    break;
                case IDM_DICTIONARY_UNLOADALL:
                        /* Unload all loaded dictionaries	*/
                    nRc = TtsUnloadAllUsrDict(hTtsInst);
                    if (nRc!= R_SUCCESSFUL)
                    {
                        MessageBox(NULL,"Unload all dictionaries failed!",NULL,MB_OK);
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
                case IDM_TTS_PLAY:
                    nRc =InputAndPlayText(hWnd);
                    if (nRc!= R_SUCCESSFUL)
                    {
                        MessageBox(NULL,"Play TTS text failed",NULL,MB_OK);
                        PostMessage(hWnd,WM_CLOSE,0,0);
                    }
                    break;
                case IDM_H_ABOUT:

                    lpfnDIALOGMsgProc = MakeProcInstance((FARPROC)About, ghInst);
                    nRc = DialogBox(ghInst, MAKEINTRESOURCE(100), hWnd, lpfnDIALOGMsgProc);
                    FreeProcInstance(lpfnDIALOGMsgProc);
                    break;
                default:
                    return DefWindowProc(hWnd, Message, wParam, lParam);

            }
            break;
        case WM_CREATE:   /* Initialize the TTS instance when the window is created. */
            nLoadedDct = 0;
                    /* Initialize the TTS instance */
            nRc  = TtsInitialize(&hTtsInst, NULL, &gSysInfo);
            if (nRc!= R_SUCCESSFUL)
            {
                MessageBox(NULL,"Initialize TTS instance failed!",NULL,MB_OK);
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
                MessageBox(NULL,"Select TTS engine failed!",NULL,MB_OK);
                PostMessage(hWnd,WM_CLOSE,0,0); /* This message also stops the TTS system. */
                break;
            }
                /* Set frequency to 11.025 kHz	*/
            TtsSetFreq(hTtsInst, FREQUENCY);
                /* Playing an introductory text */
            strcpy(szTextStr, "LHS. This is sample program 9."); 
            nRc = TtsSpeakText(hTtsInst, 1, szTextStr, strlen(szTextStr),
                               M_ANSI_TEXT, &nTaskNo);
            if (nRc!= R_SUCCESSFUL)
            {
                MessageBox(NULL,"Speak TTS text failed!",NULL,MB_OK);
                PostMessage(hWnd,WM_CLOSE,0,0); /* This message also stops the TTS system. */
            }
            break;      /* End of WM_CREATE */
        case WM_CLOSE:  /* Close the window; Uninitialize the TTS instance. */
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
        default: return DefWindowProc(hWnd, Message, wParam, lParam);
    }
    return 0L;
}


/*
 *	A dialog function to display TTS SDK product information
 */
BOOL FAR PASCAL About(HWND hWndDlg, WORD Message, WORD wParam, LONG lParam)
{
    lParam = lParam; /* To avoid warning.*/
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
