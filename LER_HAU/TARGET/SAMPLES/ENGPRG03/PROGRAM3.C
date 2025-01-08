/*
Product  :  Text-To-Speech Software Development Kit
Version  :  4.00
Program  :  PROGRAM3.c
Purpose  :  It starts playing back a hard-coded introductory text. Then it
            offers an edit field in which you can input your text. The input
            text will not be displayed as a whole but per message unit that
            is being processed by TTS. Please note that the input for this 
            sample program has to contain several message units, otherwise
            the example loses its purpose. For more information on the
            notion of splitting input texts into message units to be 
            processed by the TTS system, see the chapter on Control
            Sequences in the Text-To-Speech System User Reference Guide.
Comments :  Comments to the source code are given as they pertain to
            the usage of TTS.

Copyright (c) 1996, Lernout & Hauspie Speech Products.
All rights reserved.
*/

#include "PROGRAM3.h"
#include <stdio.h>
#include "ttssdk32.h"   /* Include this header so all TTS routines are known. */

#define FREQUENCY     M_FREQ_11KHZ       /* Use sampling rate of 11 kHz. */

HTTSINSTANCE    hTtsInst;
TTSSYSTEMINFO	gSysInfo;
int             nTaskNo;
int             nEngineId;
char            szTextStr[1024];
char            gszString[128];
char            gszAppName[20];
HINSTANCE       ghInst;
HWND            ghWndMain;
HWND            ghEditWnd;
LPSTR  	        lpstr;      /* pointer to the text */
HANDLE 	        handle;     /* handle used to capture the contents of the edit field */

static long glPlayed=0;
static FARPROC gWriteProc;

/* 
 *	Writing out the message unit being processed by TTS. 
 */
void CALLBACK WriteTTSText(HTTSINSTANCE hAppInst, 
                           UINT uCallbackType,
                           WPARAM nNumber, 
                           LPARAM lParam)
{
    char ch;
    
        /* Increase the counter for the number of characters already spoken. */
    glPlayed += nNumber;
        /* Save the next character. */
    ch = szTextStr[glPlayed];
        /* Replace the next character with a zero, to obtain a null terminated string. */
    szTextStr[glPlayed] = 0;

        /* Put the message unit in the edit field. */
    SendMessage(ghEditWnd,WM_SETTEXT,(WPARAM)NULL,(LPARAM)(LPSTR) szTextStr);
        /* This line is only needed for Microsoft Visual C++. */
    RedrawWindow(ghEditWnd, NULL, NULL, RDW_UPDATENOW | RDW_ERASENOW | RDW_INVALIDATE |RDW_ERASE);
        /* Replace the zero with the character you saved. */
    szTextStr[glPlayed] = ch;
    return;
}


/*
 *  Main Windows function
 */
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
    MSG        msg;
    RECT       Rect;
    lpszCmdLine = lpszCmdLine;

    strcpy(gszAppName, "PROGRAM3");
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
            return -2;
        }
    }

    ghWndMain = CreateWindow(
                     gszAppName,"L&H TTS Sample Program 3",WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_THICKFRAME|WS_CLIPCHILDREN|WS_OVERLAPPED,
                     CW_USEDEFAULT,0,CW_USEDEFAULT,0,NULL,NULL,ghInst,NULL);

    if(ghWndMain == NULL)
    {
        LoadString(ghInst, IDS_ERR_CREATE_WINDOW, gszString, sizeof(gszString));
        MessageBox(NULL, gszString, NULL, MB_ICONEXCLAMATION);
        return IDS_ERR_CREATE_WINDOW;
    }

    GetClientRect(ghWndMain, (LPRECT) &Rect);

        /* Create a child window. */
    ghEditWnd = CreateWindow("Edit",NULL,WS_CHILD | WS_VISIBLE |ES_MULTILINE|WS_BORDER , 0,0,
                (Rect.right-Rect.left),(Rect.bottom-Rect.top),ghWndMain,NULL,ghInst,NULL);

    if (!ghEditWnd)
    {
        DestroyWindow(ghWndMain);
        return -1;
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
 *  Main Windows Message handling function
 */
LONG FAR PASCAL WndProc(HWND hWnd, WORD Message, WORD wParam, LONG lParam)
{
    int         nRc=0, i;
    FARPROC	    lpProcGen; 
    HMENU	    hMenu;

    switch (Message)
    {
        case WM_COMMAND:
            switch (wParam)
            {
                case IDM_F_EXIT:
                    PostMessage(hWnd,WM_CLOSE,0,0);
                    break;
                case IDM_T_SPEAK:
                {
                    HANDLE handle;          /* handle used to capture the contents of the edit field */
                    LPSTR  lpstr;           /* pointer to the text */
                    DWORD  nTextLength;     /* size of the text in the edit field */

                        /* Retrieve the text from the edit field. */
                    nTextLength = SendMessage(ghEditWnd,WM_GETTEXTLENGTH,0,0);
                    if (nTextLength == 0)
                    {
                        MessageBox(NULL,"The edit field is empty!",NULL,MB_OK); /* Do not run TTS with an empty buffer. */
                        break;
                    }

                    handle = GlobalAlloc(GMEM_MOVEABLE,nTextLength+2);
                    lpstr = GlobalLock(handle);
                    SendMessage(ghEditWnd,WM_GETTEXT,(WPARAM)GlobalSize(handle),(LPARAM)lpstr);
                    SendMessage(ghEditWnd,WM_SETTEXT,0,0); /* Clean up the edit field. */

                    glPlayed=0; /* There has been no playback. */
                        /* Play the text from the edit field. */

                        /* Set callback function pointer    */
                    TtsSetCallback(hTtsInst, M_CALLBACK_TEXTUNIT, gWriteProc);
                    strcpy(szTextStr, lpstr);

                    hMenu = GetMenu(hWnd);
                    EnableMenuItem(hMenu, IDM_F_EXIT, MF_GRAYED);
                    EnableMenuItem(hMenu, IDM_T_SPEAK, MF_GRAYED);
                    EnableMenuItem(hMenu, IDM_T_GENERATE, MF_GRAYED);
                    EnableMenuItem(hMenu, IDM_H_USE, MF_GRAYED);
                    EnableMenuItem(hMenu, IDM_H_ABOUT, MF_GRAYED);
                        /* Play an input text	*/
                    nRc = TtsSpeakText(hTtsInst, 1, lpstr, strlen(lpstr),M_ANSI_TEXT, &nTaskNo);
                        /* disable callback function pointer    */
                    TtsSetCallback(hTtsInst, M_CALLBACK_TEXTUNIT, NULL);

                    EnableMenuItem(hMenu, IDM_F_EXIT, MF_ENABLED);
                    EnableMenuItem(hMenu, IDM_T_SPEAK, MF_ENABLED);
                    EnableMenuItem(hMenu, IDM_T_GENERATE, MF_ENABLED);
                    EnableMenuItem(hMenu, IDM_H_USE, MF_ENABLED);
                    EnableMenuItem(hMenu, IDM_H_ABOUT, MF_ENABLED);

                    if (nRc!= R_SUCCESSFUL)
                    {
                        MessageBox(NULL,"Speak TTS text failed!",NULL,MB_OK); /* clarify error */
                        PostMessage(hWnd,WM_CLOSE,0,0); /* This message also stops the TTS system. */
                    }
                    GlobalFree(handle);
                }
                break;
                
                case IDM_T_GENERATE:
                {
                    DWORD  nTextLength;     /* size of the text in the edit field */

                        /* Retrieve the text from the edit field. */
                    nTextLength = SendMessage(ghEditWnd,WM_GETTEXTLENGTH,0,0);
                    if (nTextLength == 0)
                    {
                        MessageBox(NULL,"The edit field is empty!",NULL,MB_OK); /* Do not run TTS with an empty buffer. */
                        break;
                    }
                    handle = GlobalAlloc(GMEM_MOVEABLE,nTextLength+2);
                    lpstr = GlobalLock(handle);
                    SendMessage(ghEditWnd,WM_GETTEXT,(WPARAM)GlobalSize(handle),(LPARAM)lpstr);
                        /* Generate a 16 bit wave file. */
                    hMenu = GetMenu(hWnd);
                    EnableMenuItem(hMenu, IDM_F_EXIT, MF_GRAYED);
                    EnableMenuItem(hMenu, IDM_T_SPEAK, MF_GRAYED);
                    EnableMenuItem(hMenu, IDM_T_GENERATE, MF_GRAYED);
                    EnableMenuItem(hMenu, IDM_H_USE, MF_GRAYED);
                    EnableMenuItem(hMenu, IDM_H_ABOUT, MF_GRAYED);

                    lpProcGen = MakeProcInstance(WaitGenFile, ghInst);
                    DialogBox(ghInst, MAKEINTRESOURCE(3000), hWnd, lpProcGen);
                    FreeProcInstance(lpProcGen);

                    EnableMenuItem(hMenu, IDM_F_EXIT, MF_ENABLED);
                    EnableMenuItem(hMenu, IDM_T_SPEAK, MF_ENABLED);
                    EnableMenuItem(hMenu, IDM_T_GENERATE, MF_ENABLED);
                    EnableMenuItem(hMenu, IDM_H_USE, MF_ENABLED);
                    EnableMenuItem(hMenu, IDM_H_ABOUT, MF_ENABLED);

                    if (nRc!= R_SUCCESSFUL)
                    {
                        MessageBox(NULL,"Generate wave file failed!",NULL,MB_OK);
                        PostMessage(hWnd,WM_CLOSE,0,0);
                    }

                    GlobalFree(handle);
                }
                break;
            
                case IDM_H_USE:
                {
                        /* Play back instructions about the use of this program. */
                    hMenu = GetMenu(hWnd);
                    EnableMenuItem(hMenu, IDM_F_EXIT, MF_GRAYED);
                    EnableMenuItem(hMenu, IDM_T_SPEAK, MF_GRAYED);
                    EnableMenuItem(hMenu, IDM_T_GENERATE, MF_GRAYED);
                    EnableMenuItem(hMenu, IDM_H_USE, MF_GRAYED);
                    EnableMenuItem(hMenu, IDM_H_ABOUT, MF_GRAYED);

                    strcpy(szTextStr, "Input some text in the edit field! And select speak or generate!");
                        /* Play an introduction text    */
                    nRc = TtsSpeakText(hTtsInst, 1, szTextStr, strlen(szTextStr),
                                       M_ANSI_TEXT, &nTaskNo);
                    if (nRc!= R_SUCCESSFUL)
                    {
                        MessageBox(NULL,"Speak TTS text failed!",NULL,MB_OK); /* clarify error */
                        PostMessage(hWnd,WM_CLOSE,0,0); /* This message also stops the TTS system. */
                    }
                    EnableMenuItem(hMenu, IDM_F_EXIT, MF_ENABLED);
                    EnableMenuItem(hMenu, IDM_T_SPEAK, MF_ENABLED);
                    EnableMenuItem(hMenu, IDM_T_GENERATE, MF_ENABLED);
                    EnableMenuItem(hMenu, IDM_H_USE, MF_ENABLED);
                    EnableMenuItem(hMenu, IDM_H_ABOUT, MF_ENABLED);
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

        case WM_CREATE: /* START the TTS system when the window is created. */
            gWriteProc = MakeProcInstance((FARPROC)WriteTTSText,ghInst);
                 /*PART 1: Initialize the TTS instance */
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
                /* Set sampling frequency to 11.025 kHz	*/
            TtsSetFreq(hTtsInst, FREQUENCY);
                /* Play an introductory text    */
            strcpy(szTextStr, "LHS. This is sample program 3.");
            nRc = TtsSpeakText(hTtsInst, 1, szTextStr, strlen(szTextStr),
                               M_ANSI_TEXT, &nTaskNo);
            if (nRc!= R_SUCCESSFUL)
            {
                MessageBox(NULL,"Speak TTS text failed!",NULL,MB_OK); /* clarify error */
                PostMessage(hWnd,WM_CLOSE,0,0); /* This message also stops the TTS system. */
                break;
            }
            break;      /* End of WM_CREATE */


        case WM_CLOSE:  /* Uninitialize the TTS instance. */
            strcpy(szTextStr, "Thank you for using the L&H TTS system!");
                /* Play the goodbye message	*/
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
 * 	A dialog function to display TTS product information
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
 * 	A dialog function to generate a PCM data file
 */
BOOL FAR PASCAL WaitGenFile(HWND hDlg, unsigned message, WORD wParam, LONG lParam)
{
    int	 nRc;

    switch (message)
    {
        case WM_INITDIALOG:
            PostMessage(hDlg, WM_GEN_SAMPLE, (WORD)hDlg, 0);
            break;
        case WM_GEN_SAMPLE:
                /* Generate a PCM data file : tts.wav   */
            nRc = TtsGenPcmFile(hTtsInst, lpstr, strlen(lpstr), 
                                "tts.wav", M_OUT_16WAVE ,
                                M_ANSI_TEXT, M_OUT_STANDARD, &nTaskNo);
            if (nRc!= R_SUCCESSFUL)
            {
                MessageBox(NULL,"Generate wave file failed!",NULL,MB_OK);
                    /* Uninitialize TTS instance   */
                TtsUninitialize(hTtsInst);
                EndDialog(hDlg, TRUE);
                break;
            }
            EndDialog(hDlg, TRUE);
            break;
     }
     return (FALSE);
}
