/*
Product  :  Text-To-Speech Software Development Kit
Version  :  4.00
Program  :  PROGRAM4.c
Purpose  :  This program illustrates how to play back synthesized speech
            from a text file. To use the program, open a file, e.g. the file
            SAMPLE.TXT which comes with the program, and then select play or
            generate.
Comments :  Comments to the source code are given as they pertain to
            the usage of TTS.

Copyright (c) 1996, Lernout & Hauspie Speech Products.
All rights reserved.
*/

#include "PROGRAM4.h"
#include <commdlg.h>   /* needed for the open file instruction */
#include <io.h>        /* needed for file reading */
#include <stdio.h>
#include "ttssdk32.h"  /* Include this header so all TTS routines are known. */

#define FREQUENCY     M_FREQ_11KHZ       /* Use sampling rate of 11 kHz. */

/*
 *  Global variables
 */
HTTSINSTANCE    hTtsInst;
TTSSYSTEMINFO   gSysInfo;
int             nTaskNo;
int             nEngineId;
HANDLE          handle;
LPSTR           lpstr;
char            gszString[128];
char            gszAppName[20];
HINSTANCE       ghInst;
HWND            ghWndMain;

        /* variables needed for the open file instruction: */
#define     LEN_DUMMY        255
char         szBuffer1[LEN_DUMMY],szBuffer2[LEN_DUMMY];
char         gszFileName[LEN_DUMMY];
char         gszFileBase[LEN_DUMMY];
OPENFILENAME OpenFileName;
char *       pBuffer;

/*
 *  Main Windows Function
 */
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
    MSG     msg;

    strcpy(gszAppName, "PROGRAM4");
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
                     gszAppName,"L&H TTS Sample Program 4",WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_THICKFRAME|WS_CLIPCHILDREN|WS_OVERLAPPED,
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
 *  Windows Message Handling function
 */
LONG FAR PASCAL WndProc(HWND hWnd, WORD Message, WORD wParam, LONG lParam)
{
    int             nRc=0, i;
    static BOOL     bFile=FALSE;
    char            szTextStr[200];
    HANDLE          hFile;
    DWORD           dwReadLen;
    HMENU           hMenu;
    FARPROC         lpProcGen;

    switch (Message)
    {
        case WM_COMMAND:
            switch (wParam)
            {
                case IDM_F_FILE: /* Retrieve the filename of the ASCII text file. */
                {
                    OpenFileName.lStructSize=sizeof(OPENFILENAME);
                    OpenFileName.hwndOwner          =hWnd;
                    OpenFileName.hInstance          =ghInst;
                    strcpy(szBuffer1,"Text Files (*.txt)");
                    pBuffer=szBuffer1+strlen(szBuffer1)+1;
                    strcpy(pBuffer,"*.txt");
                    pBuffer=pBuffer+strlen(pBuffer)+1;
                    strcpy(pBuffer,"All Files (*.*)");
                    pBuffer=pBuffer+strlen(pBuffer)+1;
                    strcpy(pBuffer,"*.*");
                    pBuffer=pBuffer+strlen(pBuffer)+1;
                    *pBuffer='\0';

                    OpenFileName.lpstrFilter        =szBuffer1;
                    szBuffer2[0] = '\0';
                    OpenFileName.lpstrCustomFilter  =szBuffer2;
                    OpenFileName.nMaxCustFilter     =LEN_DUMMY;
                    OpenFileName.nFilterIndex       =1;
                    OpenFileName.lpstrFile          =gszFileName;
                    OpenFileName.nMaxFile           =LEN_DUMMY;
                    OpenFileName.lpstrFileTitle     =gszFileBase;
                    OpenFileName.nMaxFileTitle      =LEN_DUMMY;
                    OpenFileName.Flags              =OFN_FILEMUSTEXIST;
                    OpenFileName.lpfnHook           =NULL;
                    OpenFileName.lpTemplateName     =NULL;
                    if (GetOpenFileName(&OpenFileName) !=  0) bFile = TRUE;
                    else bFile = FALSE;
                }
                break;
            case IDM_F_EXIT:
                PostMessage(hWnd,WM_CLOSE,0,0);
                break;
            case IDM_T_SPEAK:
            {
                HANDLE  handle;
                LPSTR   lpstr;
                LPSTR   lpBuf;
                DWORD   FileSize;
                DWORD   nRead=0;

                    /* Check for the existence of a file to read back. */
                if (!bFile)
                {
                    strcpy(szTextStr, "You must first select a file.");                     
                    nRc = TtsSpeakText(hTtsInst, 1, szTextStr, strlen(szTextStr),
                                       M_ANSI_TEXT, &nTaskNo);
                    if (nRc!= R_SUCCESSFUL)
                    {
                        MessageBox(NULL,"Speak TTS text failed!",NULL,MB_OK);
                        PostMessage(hWnd,WM_CLOSE,0,0); /* This message also stops the TTS system. */
                    }
                    break;
                }

                    /* Open the file and read its contents into a buffer. */
                hFile = CreateFile((LPCSTR)gszFileName, 
                                    GENERIC_READ,
                                    FILE_SHARE_READ, 
                                    (LPSECURITY_ATTRIBUTES)NULL, //&mSecAttr,
                                    OPEN_EXISTING,
                                    FILE_ATTRIBUTE_NORMAL,
                                    (HANDLE)NULL );
                if (hFile == INVALID_HANDLE_VALUE)
                    return -1;
                FileSize = (long)GetFileSize(hFile, NULL);      

                handle = GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,FileSize+2);
                if (handle == NULL)return -1;

                lpstr=GlobalLock(handle);
                if (lpstr == NULL)return -1;
                lpBuf=lpstr;
                
                nRead=ReadFile(hFile, lpBuf, (DWORD)FileSize, (LPDWORD)&dwReadLen, NULL); 
                CloseHandle(hFile);
                
                hMenu = GetMenu(hWnd);
                EnableMenuItem(hMenu, IDM_T_SPEAK, MF_GRAYED);
                EnableMenuItem(hMenu, IDM_T_GENERATE, MF_GRAYED);
                    /* Play the text from the edit field. */    
                nRc = TtsSpeakText(hTtsInst, 1, lpstr, strlen(lpstr),M_ANSI_TEXT, &nTaskNo);
                if (nRc!= R_SUCCESSFUL)
                {
                    MessageBox(NULL,"Speak TTS text failed!",NULL,MB_OK); /* clarify error */
                    PostMessage(hWnd,WM_CLOSE,0,0); /* This message also stops the TTS system. */
                }
                EnableMenuItem(hMenu, IDM_T_SPEAK, MF_ENABLED);
                EnableMenuItem(hMenu, IDM_T_GENERATE, MF_ENABLED);
                GlobalFree(handle);
            }
            break;
            
            case IDM_T_GENERATE:
            {
                LPSTR   lpBuf;
                DWORD   FileSize;
                DWORD   nRead=0;

                    /* Test for the existence of a file for which a wave file will be generated. */
                if (!bFile)
                {
                    strcpy(szTextStr, "You must first select a file.");
                        /* Play a warning text	*/
                    nRc = TtsSpeakText(hTtsInst, 1, szTextStr, strlen(szTextStr),
                                       M_ANSI_TEXT, &nTaskNo);
                    if (nRc!= R_SUCCESSFUL)
                    {
                        MessageBox(NULL,"Speak TTS text failed!",NULL,MB_OK); /* clarify error */
                        PostMessage(hWnd,WM_CLOSE,0,0); /* This message also stops the TTS system. */
                    }
                    break;
                }
                    /* Open the file and read its contents into a buffer. */
                hFile = CreateFile((LPCSTR)gszFileName, 
                                    GENERIC_READ,
                                    FILE_SHARE_READ, 
                                    (LPSECURITY_ATTRIBUTES)NULL, //&mSecAttr,
                                    OPEN_EXISTING,
                                    FILE_ATTRIBUTE_NORMAL,
                                    (HANDLE)NULL );
                if (hFile == INVALID_HANDLE_VALUE)
                    return -1;
                FileSize = (long)GetFileSize(hFile, NULL);      

                handle = GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,FileSize+2);
                if (handle == NULL)return -1;

                lpstr=GlobalLock(handle);
                if (lpstr == NULL)return -1;
                lpBuf=lpstr;
                nRead=ReadFile(hFile, lpstr, (DWORD)FileSize, (LPDWORD)&dwReadLen, NULL); 

                CloseHandle(hFile);

                    /* Generate the 16 bit wave file. */
                hMenu = GetMenu(hWnd);
                EnableMenuItem(hMenu, IDM_T_SPEAK, MF_GRAYED);
                EnableMenuItem(hMenu, IDM_T_GENERATE, MF_GRAYED);
                lpProcGen = MakeProcInstance(WaitGenFile, ghInst);
                DialogBox(ghInst, MAKEINTRESOURCE(3000), hWnd, lpProcGen);
                FreeProcInstance(lpProcGen);
                EnableMenuItem(hMenu, IDM_T_SPEAK, MF_ENABLED);
                EnableMenuItem(hMenu, IDM_T_GENERATE, MF_ENABLED);
            }
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

        case WM_CREATE:   
        
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
            nRc = TtsSelectEngine(hTtsInst, nEngineId, TRUE);

            if (nRc!= R_SUCCESSFUL)
            {
                MessageBox(NULL,"Select TTS engine failed!",NULL,MB_OK); /* clarify error */
                PostMessage(hWnd,WM_CLOSE,0,0); /* This message also stops the TTS system. */
                break;
            }
                /* Set frequency to 11.025 kHz	*/
            TtsSetFreq(hTtsInst, FREQUENCY);
                /* Play an introductory text */
            strcpy(szTextStr, "LHS. This is sample program 4.");
            nRc = TtsSpeakText(hTtsInst, 1, szTextStr, strlen(szTextStr),
                               M_ANSI_TEXT, &nTaskNo);
            if (nRc!= R_SUCCESSFUL)
            {
                MessageBox(NULL,"Speak TTS text failed!",NULL,MB_OK); /* clarify error */
                PostMessage(hWnd,WM_CLOSE,0,0); /* This message also stops the TTS system. */
            }
            break;      /* End of WM_CREATE */

        case WM_CLOSE:  /* Close the window; Uninitialize the TTS instance */
                /* Play goodbye message */
            strcpy(szTextStr, "Thank you for using the L&H TTS system!");
            nRc = TtsSpeakText(hTtsInst, 1, szTextStr, strlen(szTextStr),
                               M_ANSI_TEXT, &nTaskNo);
                /* Uninitialize the TTS instance */
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
 *	A dialog function to display TTS product information
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


/*
 *  A dialog function to generate a PCM data file
 */
BOOL FAR PASCAL WaitGenFile(HWND hDlg, unsigned message, WORD wParam, LONG lParam)
{
    int	 nRc;

    switch (message)
    {
        case WM_INITDIALOG:
            PostMessage(hDlg, WM_GEN_SAMPLE, 0, 0);
            break;
        case WM_GEN_SAMPLE:
                /* Generate a PCM data file	*/
            nRc = TtsGenPcmFile(hTtsInst, lpstr, 
                                strlen(lpstr), "tts.wav",
                                M_OUT_16WAVE, M_ANSI_TEXT, 
                                M_OUT_STANDARD,
                                &nTaskNo);
            if (nRc!= R_SUCCESSFUL)
            {
                MessageBox(NULL,"Generate wave file failed!",NULL,MB_OK); 
                    /* Uninitialize the TTS instance    */
                TtsUninitialize(hTtsInst);
                EndDialog(hDlg, TRUE);
                break;
            }
            EndDialog(hDlg, TRUE);
            break;
     }
     return (FALSE);
}
